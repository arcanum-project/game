//
//  Model.cpp
//  fragments
//
//  Created by Dmitrii Belousov on 7/10/22.
//

#include "Model.hpp"
#include "TextureController.hpp"

Model::Model(const std::vector<VertexData> vertexData, const std::vector<uint16_t> indices, MTL::Device * const pDevice, const uint16_t instanceCount, const uint16_t maxBuffersInFlight, const char * textureImgName, const char * textureImgType)
: _pDevice(pDevice),
  _vertexData(vertexData),
  _indices(indices),
  _instanceCount(instanceCount),
  _pVertexBuffer(pDevice->newBuffer(_vertexData.data(), _vertexData.size() * sizeof(VertexData), MTL::ResourceStorageModeShared)),
  _pIndexBuffer(pDevice->newBuffer(_indices.data(), _indices.size() * sizeof(uint16_t), MTL::ResourceStorageModeShared)),
  _pInstanceDataBuffer(std::vector<MTL::Buffer *>(maxBuffersInFlight)),
  _pUniformsBuffer(std::vector<MTL::Buffer *>(maxBuffersInFlight)),
  _pTexture(TextureController::instance(pDevice).makeTexture(textureImgName, textureImgType)) {
	const size_t instanceDataSize = instanceCount * sizeof(InstanceData);
	for (size_t i = 0; i < maxBuffersInFlight; i++) {
	  _pInstanceDataBuffer[i] = pDevice->newBuffer(instanceDataSize, MTL::ResourceStorageModeShared);
	}
	
	const size_t uniformsSize = MemoryAlignment::roundUpToNextMultipleOf16(sizeof(Uniforms));
	for (size_t i = 0; i < maxBuffersInFlight; i++) {
	  _pUniformsBuffer[i] = pDevice->newBuffer(uniformsSize, MTL::ResourceStorageModeShared);
	}
	
	_pVertexBuffer->setLabel(NS::String::string("Vertices", NS::UTF8StringEncoding));
	_pIndexBuffer->setLabel(NS::String::string("Indices", NS::UTF8StringEncoding));
  }

Model::~Model() {
  _pTexture->release();
  for (MTL::Buffer * const pBuffer : _pInstanceDataBuffer) {
	pBuffer->release();
  }
  _pIndexBuffer->release();
  _pVertexBuffer->release();
}
