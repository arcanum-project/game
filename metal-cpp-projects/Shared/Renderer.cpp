//
//  Renderer.cpp
//  triangle-with-animation
//
//  Created by Dmitrii Belousov on 7/1/22.
//

#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION

#include "Renderer.hpp"

Renderer::Renderer(MTL::Device * const _pDevice)
: _pDevice(_pDevice),
  _pCommandQueue(_pDevice->newCommandQueue())
{ }

Renderer::~Renderer()
{
  _pCommandQueue->release();
}

void Renderer::draw(const CA::MetalDrawable * const pDrawable) const
{
  MTL::CommandBuffer * pCmdBuf = _pCommandQueue->commandBuffer();
  
  MTL::RenderPassDescriptor * pRpd = MTL::RenderPassDescriptor::alloc()->init();
  pRpd->colorAttachments()->object(0)->setTexture(pDrawable->texture());
  pRpd->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionClear);
  pRpd->colorAttachments()->object(0)->setClearColor(MTL::ClearColor::Make(0.0, 1.0, 0.0, 1.0));
  
  MTL::RenderCommandEncoder * pEnc = pCmdBuf->renderCommandEncoder(pRpd);
  pEnc->endEncoding();
  pCmdBuf->presentDrawable(pDrawable);
  pCmdBuf->commit();
  
  pRpd->release();
}
