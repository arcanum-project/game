//
//  Model.cpp
//  fragments
//
//  Created by Dmitrii Belousov on 7/10/22.
//

#include "Model.hpp"
#include "Uniforms.hpp"
#include "Constants.hpp"

Model::Model(const std::vector<VertexData> vertexData, const std::vector<uint16_t> indices, void * const pDevice)
: _vertexData(vertexData),
  _indices(indices),
  _pVertexBuffer(reinterpret_cast<MTL::Device * const>(pDevice)->newBuffer(_vertexData.data(), _vertexData.size() * sizeof(VertexData), MTL::ResourceStorageModeShared)),
  _pIndexBuffer(reinterpret_cast<MTL::Device * const>(pDevice)->newBuffer(_indices.data(), _indices.size() * sizeof(uint16_t), MTL::ResourceStorageModeShared)) {
	assert(pDevice != nullptr);
  }

Model::~Model()
{
  _pIndexBuffer->release();
  _pVertexBuffer->release();
}

void Model::render(MTL::RenderCommandEncoder * const renderEncoder) const {
  Uniforms & uf = Uniforms::getInstance();
  uf.setModelMatrix(modelMatrix());
  renderEncoder->setVertexBytes(&uf, sizeof(Uniforms), BufferIndices::UniformsBuffer);
  
  renderEncoder->setVertexBuffer(getVertexBuffer(), 0, BufferIndices::VertexBuffer);
  
  renderEncoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, getIndices().size(), MTL::IndexTypeUInt16, getIndexBuffer(), 0);
}
