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

#pragma region Renderer {

Renderer::Renderer(MTL::Device * const _pDevice)
: _pDevice(_pDevice->retain()),
  _pCommandQueue(_pDevice->newCommandQueue()),
  _pPSO(nullptr),
  _pDepthStencilState(nullptr),
  _angle(0.f),
  _pGameScene(new GameScene(_pDevice)),
  _frame(0u),
  _semaphore(dispatch_semaphore_create(RenderingConstants::MaxBuffersInFlight)),
  _lastTimeSeconds(std::chrono::system_clock::now()) {
	buildShaders();
	buildDepthStencilState();
}

Renderer::~Renderer() {
  delete _pGameScene;
  _pDepthStencilState->release();
  _pPSO->release();
  _pCommandQueue->release();
  _pDevice->release();
}

void Renderer::buildShaders() {
  MTL::Library * pLib = _pDevice->newDefaultLibrary();
  
  MTL::Function * pVertexFn = pLib->newFunction(NS::String::string("vertex_main", NS::UTF8StringEncoding));
  MTL::Function * pFragmentFn = pLib->newFunction(NS::String::string("fragment_main", NS::UTF8StringEncoding));
  
  MTL::RenderPipelineDescriptor * pDesc = MTL::RenderPipelineDescriptor::alloc()->init();
  pDesc->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
  pDesc->setDepthAttachmentPixelFormat(MTL::PixelFormatDepth32Float);
  pDesc->setVertexFunction(pVertexFn);
  pDesc->setFragmentFunction(pFragmentFn);
  pDesc->setVertexDescriptor(VertexDescriptor::getInstance().getDefaultLayout());
  NS::Error* pError = nullptr;
  _pPSO = _pDevice->newRenderPipelineState(pDesc, &pError);
  if (!_pPSO)
  {
	__builtin_printf("%s", pError->localizedDescription()->utf8String());
  }
  
  pVertexFn->release();
  pFragmentFn->release();
  pDesc->release();
  pLib->release();
}

void Renderer::buildDepthStencilState() {
  MTL::DepthStencilDescriptor * pDepthStencilDesc = MTL::DepthStencilDescriptor::alloc()->init();
  pDepthStencilDesc->setDepthCompareFunction(MTL::CompareFunctionLess);
  pDepthStencilDesc->setDepthWriteEnabled(true);
  _pDepthStencilState = _pDevice->newDepthStencilState(pDepthStencilDesc);
  pDepthStencilDesc->release();
}

void Renderer::drawFrame(const CA::MetalDrawable * const pDrawable, const MTL::Texture * const pDepthTexture) {
  MTL::CommandBuffer * pCmdBuf = _pCommandQueue->commandBuffer();
  // We are reusing same buffers for passing tile instances data to GPU. Therefore we must lock to ensure that buffers are only used when GPU is done with them.
  // Check this article for more: https://crimild.wordpress.com/2016/05/19/praise-the-metal-part-1-rendering-a-single-frame/
  dispatch_semaphore_wait(_semaphore, DISPATCH_TIME_FOREVER);
  pCmdBuf->addCompletedHandler(^void(MTL::CommandBuffer * pCmdBuf) {
	dispatch_semaphore_signal(this->_semaphore);
  });
  
  MTL::RenderPassDescriptor * pRpd = MTL::RenderPassDescriptor::alloc()->init();
  pRpd->colorAttachments()->object(0)->setTexture(pDrawable->texture());
  pRpd->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionClear);
  pRpd->colorAttachments()->object(0)->setClearColor(MTL::ClearColor::Make(0.0, 0.5, 0.5, 1.0));
  
  MTL::RenderPassDepthAttachmentDescriptor * pRenderPassDepthAttachmentDesc = MTL::RenderPassDepthAttachmentDescriptor::alloc()->init();
  pRenderPassDepthAttachmentDesc->setTexture(pDepthTexture);
  pRpd->setDepthAttachment(pRenderPassDepthAttachmentDesc);
  
  MTL::RenderCommandEncoder * const pEnc = pCmdBuf->renderCommandEncoder(pRpd);
  pEnc->setRenderPipelineState(_pPSO);
  pEnc->setDepthStencilState(_pDepthStencilState);
  
  const std::chrono::time_point<std::chrono::system_clock> currentTimeSeconds = std::chrono::system_clock::now();
  const std::chrono::duration<float_t> deltaTime = currentTimeSeconds - _lastTimeSeconds;
  _lastTimeSeconds = currentTimeSeconds;
  _pGameScene->update(deltaTime.count());
  
  Uniforms & uf = Uniforms::getInstance();
  uf.setViewMatrix(_pGameScene->pCamera()->viewMatrix());
  uf.setProjectionMatrix(_pGameScene->pCamera()->projectionMatrix());
  pEnc->setTriangleFillMode(MTL::TriangleFillModeLines);
  _frame = (_frame + 1) % RenderingConstants::MaxBuffersInFlight;
  
  for (const std::shared_ptr<const Model> & pModel : _pGameScene->models()) {
	pModel->render(pEnc, _frame);
  }
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
