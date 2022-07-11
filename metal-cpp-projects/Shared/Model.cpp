//
//  Model.cpp
//  fragments
//
//  Created by Dmitrii Belousov on 7/10/22.
//

#include "Model.hpp"

Model::Model(const std::vector<std::array<glm::vec3, 2>> & vertexData, const std::vector<uint16_t> & indices, MTL::Device * const pDevice)
: _vertexData(vertexData),
  _indices(indices),
  _pVertexBuffer(pDevice->newBuffer(_vertexData.data(), _vertexData.size() * sizeof(std::array<glm::vec3, 2>), MTL::ResourceStorageModeShared)),
  _pIndexBuffer(pDevice->newBuffer(_indices.data(), indices.size() * sizeof(uint16_t), MTL::ResourceStorageModeShared))
{ }

Model::~Model()
{
  _pIndexBuffer->release();
  _pVertexBuffer->release();
}
