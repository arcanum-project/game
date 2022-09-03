//
//  Model.cpp
//  fragments
//
//  Created by Dmitrii Belousov on 7/10/22.
//

#include <vector>
#include <string>

#include "Uniforms.hpp"
#include "Model.hpp"
#include "Common/MemoryAlignment.hpp"

Model::Model(MTL::Device * const pDevice, const uint16_t maxBuffersInFlight)
: _pDevice(pDevice),
  _vertexData(),
  _indices(),
  _pVertexBuffer(nullptr),
  _pIndexBuffer(nullptr),
  _uniformsBuffers(std::vector<MTL::Buffer *>(maxBuffersInFlight)) {
	const size_t uniformsSize = MemoryAlignment::roundUpToNextMultipleOf16(sizeof(Uniforms));
	for (size_t i = 0; i < maxBuffersInFlight; i++) {
	  _uniformsBuffers[i] = pDevice->newBuffer(uniformsSize, MTL::ResourceStorageModeShared);
	  NS::String * const pLabel = NS::String::string("Uniforms ", NS::UTF8StringEncoding)->stringByAppendingString(NS::String::string(std::to_string(i).c_str(), NS::UTF8StringEncoding));
	  _uniformsBuffers[i]->setLabel(pLabel);
	  pLabel->release();
	}
	
	// This is to see meaningful names of buffers in debugger
	_pVertexBuffer->setLabel(NS::String::string("Vertices", NS::UTF8StringEncoding));
	_pIndexBuffer->setLabel(NS::String::string("Indices", NS::UTF8StringEncoding));
  }

Model::~Model() {
  for (MTL::Buffer * const pBuffer : _uniformsBuffers) {
	pBuffer->release();
  }
}
