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
#include "VertexDescriptor.hpp"
#include <array>
#include "glm/vec3.hpp"

#pragma region Quad {

struct Cube
{
  std::array<glm::vec3, 6> vertices =
  {
	glm::vec3(-1, 1, 0),
	glm::vec3(1, -1, 0),
	glm::vec3(-1, -1, 0),
	glm::vec3(1, 1, 0)
  };
  
  std::array<uint16_t, 6> indices =
  {
	0, 1, 2,
	0, 3, 1
  };
  
  std::array<glm::vec3, 4> colors =
  {
	glm::vec3(1, 0, 0),
	glm::vec3(0, 1, 0),
	glm::vec3(0, 0, 1),
	glm::vec3(1, 1, 0)
  };
  
  MTL::Buffer * pVertexBuffer;
  MTL::Buffer * pIndexBuffer;
  MTL::Buffer * pColorBuffer;
  
  Cube(MTL::Device * const pDevice, float scale)
  {
	for(auto & vertex : vertices)
	{
	  // Scale triangles so that they dont fit entire screen
	  vertex.x *= scale;
	  vertex.y *= scale;
	};

	pVertexBuffer = pDevice->newBuffer(&vertices, vertices.size() * sizeof(glm::vec3), MTL::ResourceStorageModeManaged);
	pIndexBuffer = pDevice->newBuffer(&indices, indices.size() * sizeof(uint16_t), MTL::ResourceStorageModeManaged);
	pColorBuffer = pDevice->newBuffer(&colors, colors.size() * sizeof(glm::vec3), MTL::ResourceStorageModeManaged);
  };
  
  ~Cube()
  {
	pVertexBuffer->release();
	pIndexBuffer->release();
	pColorBuffer->release();
  }
};

#pragma endregion Quad }

#pragma region Renderer {

Renderer::Renderer(MTL::Device * const _pDevice)
: _pDevice(_pDevice),
  _pCommandQueue(_pDevice->newCommandQueue()),
  _pPSO(nullptr),
  _angle(0)
{
  buildShaders();
}

Renderer::~Renderer()
{
  _pCommandQueue->release();
  _pPSO->release();
}

void Renderer::buildShaders()
{
  MTL::Library * pLib = _pDevice->newDefaultLibrary();
  
  MTL::Function * pVertexFn = pLib->newFunction(NS::String::string("vertex_main", NS::UTF8StringEncoding));
  MTL::Function * pFragmentFn = pLib->newFunction(NS::String::string("fragment_main", NS::UTF8StringEncoding));
  
  MTL::RenderPipelineDescriptor * pDesc = MTL::RenderPipelineDescriptor::alloc()->init();
  pDesc->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
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

void Renderer::drawFrame(const CA::MetalDrawable * const pDrawable, const MTL::Texture * const pDepthTexture)
{
  MTL::CommandBuffer * pCmdBuf = _pCommandQueue->commandBuffer();
  
  MTL::RenderPassDescriptor * pRpd = MTL::RenderPassDescriptor::alloc()->init();
  pRpd->colorAttachments()->object(0)->setTexture(pDrawable->texture());
  pRpd->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionClear);
  pRpd->colorAttachments()->object(0)->setClearColor(MTL::ClearColor::Make(0.0, 0.0, 0.0, 1.0));
  
  MTL::RenderCommandEncoder * pEnc = pCmdBuf->renderCommandEncoder(pRpd);
  // TODO creating quad every frame is suboptimal
  Cube * pQuad = new Cube(_pDevice, 0.8);
  _angle += 0.015;
  float_t currentTime = sinf(_angle);
  pEnc->setVertexBytes(&currentTime, sizeof(float_t), 11);
  pEnc->setRenderPipelineState(_pPSO);
  pEnc->setVertexBuffer(pQuad->pVertexBuffer, 0, 0);
  pEnc->setVertexBuffer(pQuad->pColorBuffer, 0, 1);
  pEnc->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, pQuad->indices.size(), MTL::IndexTypeUInt16, pQuad->pIndexBuffer, 0);
  pEnc->endEncoding();
  pCmdBuf->presentDrawable(pDrawable);
  pCmdBuf->commit();
  
  delete pQuad;
  pRpd->release();
}

#pragma endregion Renderer }
