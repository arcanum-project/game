//
//  Renderer.cpp
//  fragments
//
//  Created by Dmitrii Belousov on 7/10/22.
//

#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION

#include "Renderer.hpp"

#pragma region Renderer {

Renderer::Renderer(MTL::Device * const _pDevice)
: _pDevice(_pDevice->retain()),
  _pCommandQueue(_pDevice->newCommandQueue()),
  _pPSO(nullptr),
  _pDepthStencilState(nullptr),
  _angle(0.f),
  _pModel(ObjModelImporter().import(_pDevice, "train", "obj")) {
	buildShaders();
	buildDepthStencilState();
}

Renderer::~Renderer() {
  delete _pModel;
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
  
  MTL::RenderPassDescriptor * pRpd = MTL::RenderPassDescriptor::alloc()->init();
  pRpd->colorAttachments()->object(0)->setTexture(pDrawable->texture());
  pRpd->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionClear);
  pRpd->colorAttachments()->object(0)->setClearColor(MTL::ClearColor::Make(0.0, 0.5, 0.5, 1.0));
  
  MTL::RenderPassDepthAttachmentDescriptor * pRenderPassDepthAttachmentDesc = MTL::RenderPassDepthAttachmentDescriptor::alloc()->init();
  pRenderPassDepthAttachmentDesc->setTexture(pDepthTexture);
  pRpd->setDepthAttachment(pRenderPassDepthAttachmentDesc);
  
  MTL::RenderCommandEncoder * pEnc = pCmdBuf->renderCommandEncoder(pRpd);
  pEnc->setRenderPipelineState(_pPSO);
  pEnc->setDepthStencilState(_pDepthStencilState);
  pEnc->setVertexBuffer(_pModel->getVertexBuffer(), 0, 0);
  Uniforms & uf = Uniforms::getInstance();
  uf.setViewMatrix(glm::inverse(Math::getInstance().translation(0, 0.5, -2)));
  _angle += .005f;
  _pModel->setRotation(glm::vec3(0, sin(_angle), 0));
  uf.setModelMatrix(_pModel->getModelMatrix());
  pEnc->setVertexBytes(&uf, sizeof(Uniforms), 11);
//  pEnc->setTriangleFillMode(MTL::TriangleFillModeLines);
  pEnc->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, _pModel->getIndices().size(), MTL::IndexTypeUInt16, _pModel->getIndexBuffer(), 0);
  pEnc->endEncoding();
  pCmdBuf->presentDrawable(pDrawable);
  pCmdBuf->commit();
  
  pRenderPassDepthAttachmentDesc->release();
  pRpd->release();
}

#pragma endregion Renderer }
