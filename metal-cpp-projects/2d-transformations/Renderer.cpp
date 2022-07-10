//
//  Renderer.cpp
//  2d-transformations
//
//  Created by Dmitrii Belousov on 7/3/22.
//

#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION

#include "Renderer.hpp"
#include "VertexDescriptor.hpp"
#include <array>
#include "glm/trigonometric.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"

#pragma region Triangle {

struct Triangle
{
  std::array<glm::vec3, 3> vertices =
  {
	glm::vec3(-1, 1, 0),
	glm::vec3(1, -1, 0),
	glm::vec3(-1, -1, 0)
  };
  
  std::array<uint16_t, 3> indices =
  {
	0, 1, 2
  };
  
  std::array<glm::vec3, 3> colors =
  {
	glm::vec3(1, 0, 0),
	glm::vec3(0, 1, 0),
	glm::vec3(0, 0, 1)
  };
  
  MTL::Buffer * pVertexBuffer;
  MTL::Buffer * pIndexBuffer;
  MTL::Buffer * pColorBuffer;
  
  Triangle(MTL::Device * const pDevice, float scale)
  {
	for(auto & vertex : vertices)
	{
	  // Scale triangles so that they dont fit entire screen
	  vertex.x *= scale;
	  vertex.y *= scale;
	};

	pVertexBuffer = pDevice->newBuffer(&vertices, vertices.size() * sizeof(glm::vec3), MTL::ResourceStorageModeShared);
	pIndexBuffer = pDevice->newBuffer(&indices, indices.size() * sizeof(uint16_t), MTL::ResourceStorageModeShared);
	pColorBuffer = pDevice->newBuffer(&colors, colors.size() * sizeof(glm::vec3), MTL::ResourceStorageModeShared);
  };
  
  ~Triangle()
  {
	pVertexBuffer->release();
	pIndexBuffer->release();
	pColorBuffer->release();
  }
};

#pragma endregion Triangle }

#pragma region Math {

struct Math
{
  glm::mat4x4 identity =
  {
	glm::vec4(1, 0, 0, 0),
	glm::vec4(0, 1, 0, 0),
	glm::vec4(0, 0, 1, 0),
	glm::vec4(0, 0, 0, 1)
  };
  
  glm::mat4x4 translation(float_t trX, float_t trY, float_t trZ)
  {
	glm::mat4x4 mat = identity;
	mat[3].x = trX;
	mat[3].y = trY;
	mat[3].z = trZ;
	return mat;
  };
  
  glm::mat4x4 scaling(float_t scaleX, float_t scaleY)
  {
	glm::mat4x4 mat = identity;
	mat[0].x = scaleX;
	mat[1].y = scaleY;
	return mat;
  };
  
  glm::mat4x4 rotation(float_t angleDegrees)
  {
	float_t angleRad = glm::radians(angleDegrees);
	glm::mat4x4 mat = identity;
	mat[0].x = cosf(angleRad);
	mat[0].y = -sinf(angleRad);
	mat[1].x = sinf(angleRad);
	mat[1].y = cosf(angleRad);
	return mat;
  };
};

#pragma endregion Math }

#pragma region Renderer {

Renderer::Renderer(MTL::Device * const _pDevice)
: _pDevice(_pDevice),
  _pCommandQueue(_pDevice->newCommandQueue()),
  _pPSO(nullptr)
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

void Renderer::drawFrame(const CA::MetalDrawable * const pDrawable)
{
  MTL::CommandBuffer * pCmdBuf = _pCommandQueue->commandBuffer();
  
  MTL::RenderPassDescriptor * pRpd = MTL::RenderPassDescriptor::alloc()->init();
  pRpd->colorAttachments()->object(0)->setTexture(pDrawable->texture());
  pRpd->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionClear);
  pRpd->colorAttachments()->object(0)->setClearColor(MTL::ClearColor::Make(0.0, 0.0, 0.0, 1.0));
  
  MTL::RenderCommandEncoder * pEnc = pCmdBuf->renderCommandEncoder(pRpd);
  // TODO creating model every frame is suboptimal
  Triangle * pModel = new Triangle(_pDevice, 0.8);
  pEnc->setRenderPipelineState(_pPSO);
  pEnc->setVertexBuffer(pModel->pVertexBuffer, 0, 0);
  pEnc->setVertexBuffer(pModel->pColorBuffer, 0, 1);
  Math * pMath = new Math();
  glm::mat4x4 trMat = pMath->translation(pModel->vertices[0].x, pModel->vertices[0].y, pModel->vertices[0].z);
  glm::mat4x4 trMatInv = glm::inverse(trMat);
  glm::mat4x4 scaleMat = pMath->scaling(0.5, 0.5);
  glm::mat4x4 rotMat = pMath->rotation(90);
  pEnc->setVertexBytes(&trMat, sizeof(glm::mat4x4), 11);
  pEnc->setVertexBytes(&scaleMat, sizeof(glm::mat4x4), 12);
  pEnc->setVertexBytes(&rotMat, sizeof(glm::mat4x4), 13);
  pEnc->setVertexBytes(&trMatInv, sizeof(glm::mat4x4), 14);
  pEnc->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, pModel->indices.size(), MTL::IndexTypeUInt16, pModel->pIndexBuffer, 0);
  pEnc->endEncoding();
  pCmdBuf->presentDrawable(pDrawable);
  pCmdBuf->commit();
  
  delete pModel;
  delete pMath;
  pRpd->release();
}

#pragma endregion Renderer }
