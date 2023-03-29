//
//  Pipelines.cpp
//  iOS
//
//  Created by Dmitrii Belousov on 9/3/22.
//

#include "Pipelines.hpp"
#include "VertexDescriptor.hpp"

MTL::RenderPipelineState * Pipelines::newPSO(MTL::Device * const pDevice, MTL::Library * const pLib, const NS::String * const vertexFnName, const NS::String * const fragmentFnName, const bool enableBlending) {
  MTL::Function * pVertexFn = pLib->newFunction(vertexFnName);
  MTL::Function * pFragmentFn = pLib->newFunction(fragmentFnName);
  
  MTL::RenderPipelineDescriptor * pDesc = MTL::RenderPipelineDescriptor::alloc()->init();
  pDesc->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
  if (enableBlending) {
	pDesc->colorAttachments()->object(0)->setBlendingEnabled(enableBlending);
	pDesc->colorAttachments()->object(0)->setRgbBlendOperation(MTL::BlendOperationAdd);
	pDesc->colorAttachments()->object(0)->setAlphaBlendOperation(MTL::BlendOperationAdd);
	pDesc->colorAttachments()->object(0)->setSourceRGBBlendFactor(MTL::BlendFactorSourceAlpha);
	pDesc->colorAttachments()->object(0)->setSourceAlphaBlendFactor(MTL::BlendFactorSourceAlpha);
	pDesc->colorAttachments()->object(0)->setDestinationRGBBlendFactor(MTL::BlendFactorOneMinusSourceAlpha);
	pDesc->colorAttachments()->object(0)->setDestinationAlphaBlendFactor(MTL::BlendFactorOneMinusSourceAlpha);
  }
  pDesc->setDepthAttachmentPixelFormat(MTL::PixelFormatDepth32Float);
  pDesc->setVertexFunction(pVertexFn);
  pDesc->setFragmentFunction(pFragmentFn);
  pDesc->setVertexDescriptor(VertexDescriptor::getInstance().getDefaultLayout());
  pDesc->setSupportIndirectCommandBuffers(true);
  NS::Error * pError = nullptr;
  MTL::RenderPipelineState* pPSO = pDevice->newRenderPipelineState(pDesc, &pError);
  if (!pPSO) {
	__builtin_printf("%s", pError->localizedDescription()->utf8String());
  }
  
  pError->release();
  pDesc->release();
  pFragmentFn->release();
  pVertexFn->release();
  
  return pPSO;
}

MTL::ComputePipelineState * Pipelines::newComputePSO(MTL::Device * const pDevice, MTL::Library * const pLib, const NS::String * const fnName) {
  MTL::Function * pKernelFn = pLib->newFunction(fnName);
  NS::Error * pError = nullptr;
  MTL::ComputePipelineState* pPSO = pDevice->newComputePipelineState(pKernelFn, &pError);
  if (!pPSO) {
	__builtin_printf("%s", pError->localizedDescription()->utf8String());
  }
  
  pError->release();
  pKernelFn->release();
  
  return pPSO;
}
