//
//  Renderer.cpp
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 7/24/22.
//

#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION

#include "Renderer.hpp"
#include "Constants.h"
#include "TextureController.hpp"
#include "Pipelines.hpp"

#pragma region Renderer {

Renderer::Renderer(MTL::Device * const _pDevice)
: _pDevice(_pDevice->retain()),
  _pCommandQueue(_pDevice->newCommandQueue()),
  _pLib(_pDevice->newDefaultLibrary()),
  _pTilesPSO(nullptr),
  _pCritterPSO(nullptr),
  _pDepthStencilState(nullptr),
  _pTilesComputePSO(nullptr),
  _pIndirectCommandBuffer(nullptr),
  // Argument buffer containing the indirect command buffer encoded in the kernel
  _pIcbArgumentBuffer(nullptr),
  _pTileVisibilityKernelFn(nullptr),
  _pModelsBuffer(nullptr),
  _angle(0.f),
  _pGameScene(new GameScene(_pDevice)),
  _frame(0u),
  _semaphore(dispatch_semaphore_create(RenderingConstants::MaxBuffersInFlight)),
  _lastTimeSeconds(std::chrono::system_clock::now()) {
	buildTileShaders();
	buildCritterShaders();
	buildDepthStencilState();
	initializeTextures();
}

Renderer::~Renderer() {
  delete _pGameScene;
  _pModelsBuffer->release();
  _pTileVisibilityKernelFn->release();
  _pIcbArgumentBuffer->release();
  _pIndirectCommandBuffer->release();
  _pTilesComputePSO->release();
  _pDepthStencilState->release();
  _pCritterPSO->release();
  _pTilesPSO->release();
  _pLib->release();
  _pCommandQueue->release();
  _pDevice->release();
}

void Renderer::buildTileShaders() {
  // Make tiles render pipeline
  
  _pTilesPSO = Pipelines::newPSO(_pDevice, _pLib, NS::String::string("tileVertex", NS::UTF8StringEncoding), NS::String::string("tileFragment", NS::UTF8StringEncoding), false);
  
  // Make compute pipeline
  
  const NS::String * const kernelFnName = NS::String::string("cullTilesAndEncodeCommands", NS::UTF8StringEncoding);
  _pTileVisibilityKernelFn = _pLib->newFunction(kernelFnName);
  _pTilesComputePSO = Pipelines::newComputePSO(_pDevice, _pLib, kernelFnName);
  
  // Make indirect command buffer
  
  MTL::IndirectCommandBufferDescriptor * const pIcbDescriptor = MTL::IndirectCommandBufferDescriptor::alloc()->init();
  pIcbDescriptor->setCommandTypes(MTL::IndirectCommandTypeDrawIndexed);
  // Indicate that buffers will be set for each command in the indirect command buffer.
  pIcbDescriptor->setInheritBuffers(false);
  // Indicate that a maximum of 3 buffers will be set for each command.
  pIcbDescriptor->setMaxVertexBufferBindCount(25);
  pIcbDescriptor->setMaxFragmentBufferBindCount(25);
#if defined TARGET_MACOS || defined(__IPHONE_13_0)
  // Indicate that the render pipeline state object will be set in the render command encoder
  // (not by the indirect command buffer).
  // On iOS, this property only exists on iOS 13 and later.  Earlier versions of iOS did not
  // support settings pipelinestate within an indirect command buffer, so indirect command
  // buffers always inherited the pipeline state.
  pIcbDescriptor->setInheritPipelineState(true);
#endif
  
  // Create indirect command buffer using private storage mode; since only the GPU will
  // write to and read from the indirect command buffer, the CPU never needs to access the
  // memory
  _pIndirectCommandBuffer = _pDevice->newIndirectCommandBuffer(pIcbDescriptor, RenderingConstants::NumOfTilesPerSector, MTL::ResourceStorageModeShared);
  _pIndirectCommandBuffer->setLabel(NS::String::string("Scene ICB", NS::UTF8StringEncoding));
  
  pIcbDescriptor->release();
  
  // Make ICB Argument buffer
  // Argument buffer containing the indirect command buffer encoded in the kernel
  
  MTL::ArgumentEncoder * const pArgumentEncoder = _pTileVisibilityKernelFn->newArgumentEncoder(BufferIndices::ICBBuffer);
  _pIcbArgumentBuffer = _pDevice->newBuffer(pArgumentEncoder->encodedLength(), MTL::ResourceStorageModeShared);
  _pIcbArgumentBuffer->setLabel(NS::String::string("ICB Argument Buffer", NS::UTF8StringEncoding));
  pArgumentEncoder->setArgumentBuffer(_pIcbArgumentBuffer, 0);
  pArgumentEncoder->setIndirectCommandBuffer(_pIndirectCommandBuffer, BufferIndices::ICBArgumentsBuffer);
  
  pArgumentEncoder->release();
}

void Renderer::buildCritterShaders() {
  _pCritterPSO = Pipelines::newPSO(_pDevice, _pLib, NS::String::string("characterVertex", NS::UTF8StringEncoding), NS::String::string("characterFragment", NS::UTF8StringEncoding), true);
}

void Renderer::buildDepthStencilState() {
  MTL::DepthStencilDescriptor * pDepthStencilDesc = MTL::DepthStencilDescriptor::alloc()->init();
  pDepthStencilDesc->setDepthCompareFunction(MTL::CompareFunctionLess);
  pDepthStencilDesc->setDepthWriteEnabled(true);
  _pDepthStencilState = _pDevice->newDepthStencilState(pDepthStencilDesc);
  pDepthStencilDesc->release();
}

void Renderer::initializeTextures() {
  TextureController & txController = TextureController::instance(_pDevice);
  txController.makeHeap();
  txController.moveTexturesToHeap(_pCommandQueue);
  
  MTL::ArgumentEncoder * const pArgumentEncoder = _pTileVisibilityKernelFn->newArgumentEncoder(BufferIndices::TextureBuffer);
  _pModelsBuffer = _pDevice->newBuffer(pArgumentEncoder->encodedLength(), MTL::ResourceStorageModeShared);
  pArgumentEncoder->setArgumentBuffer(_pModelsBuffer, 0);
  pArgumentEncoder->setTextures(txController.textures().data(), NS::Range(0, txController.textures().size()));
  pArgumentEncoder->setTexture(txController.textures().at(0), 0);
  
  pArgumentEncoder->release();
}

void Renderer::drawFrame(const CA::MetalDrawable * const pDrawable, const MTL::Texture * const pDepthTexture) {
  // We are reusing same buffers for passing tile instances data to GPU. Therefore we must lock to ensure that buffers are only used when GPU is done with them.
  // Check this article for more: https://crimild.wordpress.com/2016/05/19/praise-the-metal-part-1-rendering-a-single-frame/
  dispatch_semaphore_wait(_semaphore, DISPATCH_TIME_FOREVER);
  
  MTL::CommandBuffer * pCmdBuf = _pCommandQueue->commandBuffer();
  pCmdBuf->addCompletedHandler(^void(MTL::CommandBuffer * pCmdBuf) {
	dispatch_semaphore_signal(this->_semaphore);
  });
  
  // Encode command to reset the indirect command buffer
  MTL::BlitCommandEncoder * const pResetBlitEncoder = pCmdBuf->blitCommandEncoder();
  pResetBlitEncoder->setLabel(NS::String::string("Reset ICB Blit Encoder", NS::UTF8StringEncoding));
  pResetBlitEncoder->resetCommandsInBuffer(_pIndirectCommandBuffer, NS::Range(0, RenderingConstants::NumOfTilesPerSector));
  pResetBlitEncoder->endEncoding();
  
  const std::chrono::time_point<std::chrono::system_clock> currentTimeSeconds = std::chrono::system_clock::now();
  const std::chrono::duration<float_t> deltaTime = currentTimeSeconds - _lastTimeSeconds;
  _lastTimeSeconds = currentTimeSeconds;
  _pGameScene->update(deltaTime.count());
  
  Uniforms & uf = Uniforms::getInstance();
  uf.setViewMatrix(_pGameScene->pCamera()->viewMatrix());
  uf.setProjectionMatrix(_pGameScene->pCamera()->projectionMatrix());
  _frame = (_frame + 1) % RenderingConstants::MaxBuffersInFlight;
  
  // Encode commands to determine visibility of tiles using a compute kernel
  MTL::ComputeCommandEncoder * const pTileComputeEncoder = pCmdBuf->computeCommandEncoder();
  pTileComputeEncoder->setLabel(NS::String::string("Tile Visibility Kernel", NS::UTF8StringEncoding));
  pTileComputeEncoder->setComputePipelineState(_pTilesComputePSO);

  for (const std::shared_ptr<Model> & pModel : _pGameScene->models()) {
	pModel->render(pTileComputeEncoder, _frame);
  }
  pTileComputeEncoder->setBuffer(_pIcbArgumentBuffer, 0, BufferIndices::ICBBuffer);
  pTileComputeEncoder->setBuffer(_pModelsBuffer, 0, BufferIndices::TextureBuffer);
  // Call useResource on '_indirectCommandBuffer' which indicates to Metal that the kernel will
  // access '_indirectCommandBuffer'.  It is necessary because the app cannot directly set
  // '_indirectCommandBuffer' in 'computeEncoder', but, rather, must pass it to the kernel via
  // an argument buffer which indirectly contains '_indirectCommandBuffer'.
  pTileComputeEncoder->useResource(_pIndirectCommandBuffer, MTL::ResourceUsageWrite);
  pTileComputeEncoder->useHeap(TextureController::instance(_pDevice).heap());
  const uint64_t threadExecutionWidth = _pTilesComputePSO->threadExecutionWidth();
  pTileComputeEncoder->dispatchThreads(MTL::Size(RenderingConstants::NumOfTilesPerSector, 1, 1), MTL::Size(threadExecutionWidth, 1, 1));
  pTileComputeEncoder->endEncoding();
  
  // Encode command to optimize the indirect command buffer after encoding
  MTL::BlitCommandEncoder * const pOptimizeBlitEncoder = pCmdBuf->blitCommandEncoder();
  pOptimizeBlitEncoder->setLabel(NS::String::string("Optimize ICB Blit Encoder", NS::UTF8StringEncoding));
  pOptimizeBlitEncoder->optimizeIndirectCommandBuffer(_pIndirectCommandBuffer, NS::Range(0, RenderingConstants::NumOfTilesPerSector));
  pOptimizeBlitEncoder->endEncoding();
  
  MTL::RenderPassDescriptor * pTileRpd = MTL::RenderPassDescriptor::alloc()->init();
  pTileRpd->colorAttachments()->object(0)->setTexture(pDrawable->texture());
  pTileRpd->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionClear);
  pTileRpd->colorAttachments()->object(0)->setClearColor(MTL::ClearColor::Make(0.0f, 0.0f, 1.0f, 1.0));
  
  MTL::RenderPassDepthAttachmentDescriptor * pRenderPassDepthAttachmentDesc = MTL::RenderPassDepthAttachmentDescriptor::alloc()->init();
  pRenderPassDepthAttachmentDesc->setTexture(pDepthTexture);
  pTileRpd->setDepthAttachment(pRenderPassDepthAttachmentDesc);

  MTL::RenderCommandEncoder * const pTileRenderEncoder = pCmdBuf->renderCommandEncoder(pTileRpd);
  pTileRenderEncoder->setLabel(NS::String::string("Tile Render Encoder", NS::UTF8StringEncoding));
  pTileRenderEncoder->setRenderPipelineState(_pTilesPSO);
  pTileRenderEncoder->setDepthStencilState(_pDepthStencilState);
  pTileRenderEncoder->executeCommandsInBuffer(_pIndirectCommandBuffer, NS::Range(0, RenderingConstants::NumOfTilesPerSector));
  pTileRenderEncoder->endEncoding();
  
  MTL::RenderPassDescriptor * pCritterRpd = MTL::RenderPassDescriptor::alloc()->init();
  pCritterRpd->colorAttachments()->object(0)->setTexture(pDrawable->texture());
  pCritterRpd->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionLoad);
  pCritterRpd->setDepthAttachment(pRenderPassDepthAttachmentDesc);
  
  MTL::RenderCommandEncoder * const pCritterRenderEncoder = pCmdBuf->renderCommandEncoder(pCritterRpd);
  pCritterRenderEncoder->setLabel(NS::String::string("Critter Render Encoder", NS::UTF8StringEncoding));
  pCritterRenderEncoder->setRenderPipelineState(_pCritterPSO);
  pCritterRenderEncoder->setDepthStencilState(_pDepthStencilState);
  pCritterRenderEncoder->setFragmentBuffer(_pModelsBuffer, 0, BufferIndices::TextureBuffer);
  pCritterRenderEncoder->useHeap(TextureController::instance(_pDevice).heap());
//  _pGameScene->pCharacter()->render(pCritterRenderEncoder, 0);
  pCritterRenderEncoder->endEncoding();
  
  pCmdBuf->presentDrawable(pDrawable);
  pCmdBuf->commit();
  
  pRenderPassDepthAttachmentDesc->release();
  pTileRpd->release();
  pCritterRpd->release();
}

void Renderer::drawableSizeWillChange(const float_t & drawableWidth, const float_t & drawableHeight) {
  Uniforms & uf = Uniforms::getInstance();
  uf.setDrawableWidth(drawableWidth);
  uf.setDrawableHeight(drawableHeight);
  _pGameScene->update(uf.drawableWidth(), uf.drawableHeight());
}

#pragma endregion Renderer }
