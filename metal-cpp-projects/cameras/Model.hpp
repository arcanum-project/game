//
//  Model.h
//  cameras
//
//  Created by Dmitrii Belousov on 7/9/22.
//

#pragma once

#include "Metal/Metal.hpp"
#include "glm/vec3.hpp"
#include "Transformable.hpp"

#pragma region Model {

struct Model : public Transformable
{
  // Cube
  std::array<glm::vec3, 8> vertices =
  {
	glm::vec3(-0.5f, 0.25f, -0.5f),  // 0
	glm::vec3(-0.5f, -0.25f, -0.5f),  // 1
	glm::vec3(0.5f, -0.25f, -0.5f),   // 2
	glm::vec3(0.5f, 0.25f, -0.5f),   // 3
	glm::vec3(-0.5f, 0.25f, 0.5f),  // 4
	glm::vec3(-0.5f, -0.25f, 0.5f),  // 5
	glm::vec3(0.5f, -0.25f, 0.5f),   // 6
	glm::vec3(0.5f, 0.25f, 0.5f)    // 7
  };
  
  std::array<uint16_t, 36> indices =
  {
	// south
	0, 1, 2,
	2, 3, 0,
	// north
	4, 5, 6,
	6, 7, 4,
	// west
	4, 5, 1,
	1, 0, 4,
	// east
	3, 2, 6,
	6, 7, 3,
	// top
	4, 0, 3,
	3, 7, 4,
	// bottom
	5, 1, 2,
	2, 6, 5
  };
  
  std::array<glm::vec3, 8> colors =
  {
	glm::vec3(0, 0, 1),
	glm::vec3(0, 0, 1),
	glm::vec3(0, 0, 1),
	glm::vec3(0, 0, 1),
	glm::vec3(0, 0, 1),
	glm::vec3(0, 0, 1),
	glm::vec3(0, 0, 1),
	glm::vec3(0, 0, 1)
  };
  
  MTL::Buffer * pVertexBuffer;
  MTL::Buffer * pIndexBuffer;
  MTL::Buffer * pColorBuffer;
  
  Model(MTL::Device * const pDevice)
  : pVertexBuffer(nullptr),
	pIndexBuffer(nullptr),
	pColorBuffer(nullptr)
  {
	pVertexBuffer = pDevice->newBuffer(&vertices, vertices.size() * sizeof(glm::vec3), MTL::ResourceStorageModeShared);
	pIndexBuffer = pDevice->newBuffer(&indices, indices.size() * sizeof(uint16_t), MTL::ResourceStorageModeShared);
	pColorBuffer = pDevice->newBuffer(&colors, colors.size() * sizeof(glm::vec3), MTL::ResourceStorageModeShared);
  };
  
  ~Model()
  {
	pColorBuffer->release();
	pIndexBuffer->release();
	pVertexBuffer->release();
  }
};

#pragma endregion Model }
