//
//  Model.cpp
//  fragments
//
//  Created by Dmitrii Belousov on 7/10/22.
//

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <string_view>
#include <vector>
#include <string>
#include <unordered_map>

#include "Model.hpp"
#include "TextureController.hpp"

Model::Model(const std::vector<VertexData> vertexData, const std::vector<uint16_t> indices, MTL::Device * const pDevice, const uint16_t instanceCount, const uint16_t maxBuffersInFlight, const char * textureImgName, const char * textureImgType)
: _pDevice(pDevice),
  _vertexData(vertexData),
  _indices(indices),
  _instanceCount(instanceCount),
  _pVertexBuffer(pDevice->newBuffer(_vertexData.data(), _vertexData.size() * sizeof(VertexData), MTL::ResourceStorageModeShared)),
  _pIndexBuffer(pDevice->newBuffer(_indices.data(), _indices.size() * sizeof(uint16_t), MTL::ResourceStorageModeShared)),
  _pInstanceDataBuffers(std::vector<MTL::Buffer *>(maxBuffersInFlight)),
  _pUniformsBuffers(std::vector<MTL::Buffer *>(maxBuffersInFlight)),
  _instanceIdToTextureIndex() {
	const size_t instanceDataSize = instanceCount * MemoryAlignment::roundUpToNextMultipleOf16(sizeof(InstanceData));
	for (size_t i = 0; i < maxBuffersInFlight; i++) {
	  _pInstanceDataBuffers[i] = pDevice->newBuffer(instanceDataSize, MTL::ResourceStorageModeShared);
	}
	
	const size_t uniformsSize = MemoryAlignment::roundUpToNextMultipleOf16(sizeof(Uniforms));
	for (size_t i = 0; i < maxBuffersInFlight; i++) {
	  _pUniformsBuffers[i] = pDevice->newBuffer(uniformsSize, MTL::ResourceStorageModeShared);
	}
	
	loadTextures();
	
	// This is to see meaningful names of buffers in debugger
	_pVertexBuffer->setLabel(NS::String::string("Vertices", NS::UTF8StringEncoding));
	_pIndexBuffer->setLabel(NS::String::string("Indices", NS::UTF8StringEncoding));
  }

Model::~Model() {
  for (MTL::Buffer * const pBuffer : _pUniformsBuffers) {
	pBuffer->release();
  }
  for (MTL::Buffer * const pBuffer : _pInstanceDataBuffers) {
	pBuffer->release();
  }
  _pIndexBuffer->release();
  _pVertexBuffer->release();
}

/**
 Load terrain textures for crash site sector.
 */
void Model::loadTextures() {
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ResourceBundle::absolutePath("86570436012", ""), pt);
  const boost::property_tree::ptree tilesArr = pt.get_child("tiles");
  // Iterator to iterate between array items (aka tiles)
  for (boost::property_tree::ptree::const_iterator arrItemsIterator = tilesArr.begin(); arrItemsIterator != tilesArr.end(); ++arrItemsIterator) {
	uint16_t instanceId {};
	std::string textureName {};
	// Iterator to move between fields inside a specific array item (aka tile)
	for (boost::property_tree::ptree::const_iterator arrItemIterator = arrItemsIterator->second.begin(); arrItemIterator != arrItemsIterator->second.end(); ++arrItemIterator) {
	  const std::string key = arrItemIterator->first;
	  if (key == "instanceId") {
		instanceId = boost::lexical_cast<uint16_t>(arrItemIterator->second.data());
	  } else if (key == "textureName") {
		textureName = arrItemIterator->second.data();
		const uint16_t textureIndex = TextureController::instance(_pDevice).loadTexture(textureName.c_str(), "bmp");
		_instanceIdToTextureIndex.insert(std::make_pair(instanceId, textureIndex));
	  } else
		throw std::runtime_error("Unknown tile array format");
	}
  }
}
