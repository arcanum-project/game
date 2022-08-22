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
#include "Constants.hpp"
#include "TextureController.hpp"

#pragma region Renderer {

Renderer::Renderer(MTL::Device * const _pDevice)
: _pDevice(_pDevice->retain()),
  _pCommandQueue(_pDevice->newCommandQueue()),
  _pPSO(nullptr),
  _pDepthStencilState(nullptr),
  _pComputePipelineState(nullptr),
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
	buildShaders();
	buildDepthStencilState();
	initializeModels();
}

Renderer::~Renderer() {
  delete _pGameScene;
  _pModelsBuffer->release();
  _pTileVisibilityKernelFn->release();
  _pIcbArgumentBuffer->release();
  _pIndirectCommandBuffer->release();
  _pComputePipelineState->release();
  _pDepthStencilState->release();
  _pPSO->release();
  _pCommandQueue->release();
  _pDevice->release();
}

void Renderer::buildShaders() {
  MTL::Library * pLib = _pDevice->newDefaultLibrary();
  
  // Make render pipeline
  
  MTL::Function * pVertexFn = pLib->newFunction(NS::String::string("vertex_main", NS::UTF8StringEncoding));
  MTL::Function * pFragmentFn = pLib->newFunction(NS::String::string("fragment_main", NS::UTF8StringEncoding));
  
  MTL::RenderPipelineDescriptor * pDesc = MTL::RenderPipelineDescriptor::alloc()->init();
  pDesc->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
  pDesc->setDepthAttachmentPixelFormat(MTL::PixelFormatDepth32Float);
  pDesc->setVertexFunction(pVertexFn);
  pDesc->setFragmentFunction(pFragmentFn);
  pDesc->setVertexDescriptor(VertexDescriptor::getInstance().getDefaultLayout());
  pDesc->setSupportIndirectCommandBuffers(true);
  NS::Error * pError = nullptr;
  _pPSO = _pDevice->newRenderPipelineState(pDesc, &pError);
  if (!_pPSO) {
	__builtin_printf("%s", pError->localizedDescription()->utf8String());
  }
  
  pVertexFn->release();
  pFragmentFn->release();
  pDesc->release();
  
  // Make compute pipeline
  
  _pTileVisibilityKernelFn = pLib->newFunction(NS::String::string("cullTilesAndEncodeCommands", NS::UTF8StringEncoding));
  
  _pComputePipelineState = _pDevice->newComputePipelineState(_pTileVisibilityKernelFn, &pError);
  if (!_pComputePipelineState) {
	__builtin_printf("%s", pError->localizedDescription()->utf8String());
  }
  
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
  pLib->release();
}

void Renderer::buildDepthStencilState() {
  MTL::DepthStencilDescriptor * pDepthStencilDesc = MTL::DepthStencilDescriptor::alloc()->init();
  pDepthStencilDesc->setDepthCompareFunction(MTL::CompareFunctionLess);
  pDepthStencilDesc->setDepthWriteEnabled(true);
  _pDepthStencilState = _pDevice->newDepthStencilState(pDepthStencilDesc);
  pDepthStencilDesc->release();
}

void Renderer::initializeModels() {
  TextureController & txController = TextureController::instance(_pDevice);
  txController.makeHeap();
  txController.moveTexturesToHeap(_pCommandQueue);
  
  MTL::ArgumentEncoder * const pArgumentEncoder = _pTileVisibilityKernelFn->newArgumentEncoder(BufferIndices::ModelsBuffer);
  _pModelsBuffer = _pDevice->newBuffer(pArgumentEncoder->encodedLength(), MTL::ResourceStorageModeShared);
  pArgumentEncoder->setArgumentBuffer(_pModelsBuffer, 0);
  pArgumentEncoder->setTextures(txController.textures().data(), NS::Range(0, txController.textures().size()));
//  for (uint16_t i = 0; i < txController.textures().size(); i++) {
//	pArgumentEncoder->setTexture(txController.textures().at(i), i);
//  }
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
  MTL::ComputeCommandEncoder * const pComputeEncoder = pCmdBuf->computeCommandEncoder();
  pComputeEncoder->setLabel(NS::String::string("Tile Visibility Kernel", NS::UTF8StringEncoding));
  pComputeEncoder->setComputePipelineState(_pComputePipelineState);

  for (const std::shared_ptr<const Model> & pModel : _pGameScene->models()) {
	pModel->render(pComputeEncoder, _frame);
  }
  pComputeEncoder->setBuffer(_pIcbArgumentBuffer, 0, BufferIndices::ICBBuffer);
  pComputeEncoder->setBuffer(_pModelsBuffer, 0, BufferIndices::ModelsBuffer);
  // Call useResource on '_indirectCommandBuffer' which indicates to Metal that the kernel will
  // access '_indirectCommandBuffer'.  It is necessary because the app cannot directly set
  // '_indirectCommandBuffer' in 'computeEncoder', but, rather, must pass it to the kernel via
  // an argument buffer which indirectly contains '_indirectCommandBuffer'.
  pComputeEncoder->useResource(_pIndirectCommandBuffer, MTL::ResourceUsageWrite);
  pComputeEncoder->useHeap(TextureController::instance(_pDevice).heap());
  const uint64_t threadExecutionWidth = _pComputePipelineState->threadExecutionWidth();
  pComputeEncoder->dispatchThreads(MTL::Size(RenderingConstants::NumOfTilesPerSector, 1, 1), MTL::Size(threadExecutionWidth, 1, 1));
  pComputeEncoder->endEncoding();
  
  // Encode command to optimize the indirect command buffer after encoding
  MTL::BlitCommandEncoder * const pOptimizeBlitEncoder = pCmdBuf->blitCommandEncoder();
  pOptimizeBlitEncoder->setLabel(NS::String::string("Optimize ICB Blit Encoder", NS::UTF8StringEncoding));
  pOptimizeBlitEncoder->optimizeIndirectCommandBuffer(_pIndirectCommandBuffer, NS::Range(0, RenderingConstants::NumOfTilesPerSector));
  pOptimizeBlitEncoder->endEncoding();
  
  MTL::RenderPassDescriptor * pRpd = MTL::RenderPassDescriptor::alloc()->init();
  pRpd->colorAttachments()->object(0)->setTexture(pDrawable->texture());
  pRpd->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionClear);
  pRpd->colorAttachments()->object(0)->setClearColor(MTL::ClearColor::Make(0.0f, 0.0f, 1.0f, 1.0));
  
  MTL::RenderPassDepthAttachmentDescriptor * pRenderPassDepthAttachmentDesc = MTL::RenderPassDepthAttachmentDescriptor::alloc()->init();
  pRenderPassDepthAttachmentDesc->setTexture(pDepthTexture);
  pRpd->setDepthAttachment(pRenderPassDepthAttachmentDesc);

  MTL::RenderCommandEncoder * const pEnc = pCmdBuf->renderCommandEncoder(pRpd);
  pEnc->setRenderPipelineState(_pPSO);
  pEnc->setDepthStencilState(_pDepthStencilState);
//  pEnc->setTriangleFillMode(MTL::TriangleFillModeLines);
  pEnc->executeCommandsInBuffer(_pIndirectCommandBuffer, NS::Range(0, RenderingConstants::NumOfTilesPerSector));
  pEnc->endEncoding();
  
  pCmdBuf->presentDrawable(pDrawable);
  pCmdBuf->commit();
  
  pRenderPassDepthAttachmentDesc->release();
  pRpd->release();
}

void Renderer::drawableSizeWillChange(const float_t & drawableWidth, const float_t & drawableHeight) {
  Uniforms & uf = Uniforms::getInstance();
  uf.setDrawableWidth(drawableWidth);
  uf.setDrawableHeight(drawableHeight);
  _pGameScene->update(uf.drawableWidth(), uf.drawableHeight());
}

#pragma endregion Renderer }
