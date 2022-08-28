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
#include "ObjModelImporter.hpp"

Model::Model(MTL::Device * const pDevice, const uint16_t instanceCount, const uint16_t maxBuffersInFlight)
: _pDevice(pDevice),
  _vertexData(),
  _flippedVertexData(),
  _indices(),
  _instanceCount(instanceCount),
  _pVertexBuffer(nullptr),
  _pFlippedVertexBuffer(nullptr),
  _pIndexBuffer(nullptr),
  _instanceDataBuffers(std::vector<MTL::Buffer *>(maxBuffersInFlight)),
  _uniformsBuffers(std::vector<MTL::Buffer *>(maxBuffersInFlight)),
  _instanceIdToData() {
	populateVertexData();
	
	const size_t instanceDataSize = instanceCount * MemoryAlignment::roundUpToNextMultipleOf16(sizeof(InstanceData));
	for (size_t i = 0; i < maxBuffersInFlight; i++) {
	  _instanceDataBuffers[i] = pDevice->newBuffer(instanceDataSize, MTL::ResourceStorageModeShared);
	}
	
	const size_t uniformsSize = MemoryAlignment::roundUpToNextMultipleOf16(sizeof(Uniforms));
	for (size_t i = 0; i < maxBuffersInFlight; i++) {
	  _uniformsBuffers[i] = pDevice->newBuffer(uniformsSize, MTL::ResourceStorageModeShared);
	}
	
	loadTextures();
	
	// This is to see meaningful names of buffers in debugger
	_pVertexBuffer->setLabel(NS::String::string("Vertices", NS::UTF8StringEncoding));
	_pIndexBuffer->setLabel(NS::String::string("Indices", NS::UTF8StringEncoding));
  }

Model::~Model() {
  for (MTL::Buffer * const pBuffer : _uniformsBuffers) {
	pBuffer->release();
  }
  for (MTL::Buffer * const pBuffer : _instanceDataBuffers) {
	pBuffer->release();
  }
  _pIndexBuffer->release();
  _pFlippedVertexBuffer->release();
  _pVertexBuffer->release();
}

void Model::populateVertexData() {
  const std::unique_ptr<const ImportedModelData> tile = ObjModelImporter().import("tile", "obj");
  _vertexData = tile->vertexData;
  _indices = tile->indices;
  const std::unique_ptr<const ImportedModelData> tileFlipped = ObjModelImporter().import("tile-flipped", "obj");
  _flippedVertexData = tileFlipped->vertexData;
  
  _pVertexBuffer = _pDevice->newBuffer(_vertexData.data(), _vertexData.size() * sizeof(VertexData), MTL::ResourceStorageModeShared);
  _pFlippedVertexBuffer = _pDevice->newBuffer(_flippedVertexData.data(), _flippedVertexData.size() * sizeof(VertexData), MTL::ResourceStorageModeShared);
  _pIndexBuffer = _pDevice->newBuffer(_indices.data(), _indices.size() * sizeof(uint16_t), MTL::ResourceStorageModeShared);
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
	InstanceData data {};
	// Iterator to move between fields inside a specific array item (aka tile)
	for (boost::property_tree::ptree::const_iterator arrItemIterator = arrItemsIterator->second.begin(); arrItemIterator != arrItemsIterator->second.end(); ++arrItemIterator) {
	  const std::string key = arrItemIterator->first;
	  if (key.compare("instanceId") == 0) {
		instanceId = boost::lexical_cast<uint16_t>(arrItemIterator->second.data());
	  } else if (key.compare("textureName") == 0) {
		textureName = arrItemIterator->second.data();
		const uint16_t textureIndex = TextureController::instance(_pDevice).loadTexture(textureName.c_str(), "bmp");
		data.textureIndex = textureIndex;
	  } else if (key.compare("shouldFlip") == 0) {
		bool shouldFlip = boost::lexical_cast<bool>(arrItemIterator->second.data());
		data.shouldFlip = shouldFlip;
		_instanceIdToData.insert(std::make_pair(instanceId, data));
	  } else
		throw std::runtime_error("Unknown tile array format");
	}
  }
}
