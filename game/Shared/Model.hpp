//
//  Model.hpp
//  fragments
//
//  Created by Dmitrii Belousov on 7/10/22.
//

#pragma once

#include <Metal/Metal.hpp>
#include <vector>
#include <TargetConditionals.h>
#include <unordered_map>
#include <string>

#include "Transformable.hpp"
#include "VertexData.hpp"
#include "Uniforms.hpp"
#include "Constants.hpp"
#include "MemoryAlignment.hpp"

struct InstanceData {
  glm::mat4x4 instanceTransform;
  uint16_t textureIndex;
  bool shouldFlip;
  // Padding to ensure that sizeof(InstanceData) returns the size of the struct that we allocated memory for
  char pad[11];
};

class Model : public Transformable
{
public:
  Model(MTL::Device * const pDevice, const uint16_t instanceCount, const uint16_t maxBuffersInFlight);
  ~Model();
  
  inline const std::vector<uint16_t> & indices() const { return _indices; }
  inline const MTL::Buffer * const pVertexBuffer() const { return _pVertexBuffer; }
  inline const MTL::Buffer * const pFlippedVertexBuffer() const { return _pFlippedVertexBuffer; }
  inline const MTL::Buffer * const pIndexBuffer() const { return _pIndexBuffer; }
  inline const std::vector<MTL::Buffer *> instanceDataBuffers() const { return _instanceDataBuffers; }
  inline const std::vector<MTL::Buffer *> uniformsBuffers() const { return _uniformsBuffers; }
  
  inline void render(MTL::ComputeCommandEncoder * const pComputeEncoder, const uint16_t & frame) const {
	MTL::Buffer * const pInstanceDataBuffer = _instanceDataBuffers.at(frame);
	InstanceData * pInstanceData = reinterpret_cast<InstanceData *>(pInstanceDataBuffer->contents());
	for (size_t i = 0; i < RenderingConstants::NumOfTilesPerSector; ++i) {
	  const float_t rowOffset = (float_t) (i % (RenderingConstants::NumOfTilesPerSector / RenderingConstants::NumOfTilesPerRow));
	  const float_t columnOffset = (float_t) (i / (RenderingConstants::NumOfTilesPerSector / RenderingConstants::NumOfTilesPerRow));
	  pInstanceData[i].instanceTransform = Math::getInstance().translation(rowOffset * 2.0f, 0.0f, columnOffset * 2.0f);
	  const std::unordered_map<uint16_t, InstanceData>::const_iterator iterator = _instanceIdToData.find(i);
	  if (iterator == _instanceIdToData.end())
		throw std::runtime_error("Texture index not found for instanceId. instanceId = " + std::to_string(i));
	  pInstanceData[i].textureIndex = iterator->second.textureIndex;
	  pInstanceData[i].shouldFlip = iterator->second.shouldFlip;
	}
#if defined(TARGET_OSX)
	pInstanceDataBuffer->didModifyRange(NS::Range::Make(0, pInstanceDataBuffer->length()));
#endif
	
	Uniforms & uf = Uniforms::getInstance();
	uf.setModelMatrix(modelMatrix());
	MTL::Buffer * const pUniformsBuffer = _uniformsBuffers.at(frame);
	memcpy(pUniformsBuffer->contents(), & uf, MemoryAlignment::roundUpToNextMultipleOf16(sizeof(Uniforms)));
#if defined(TARGET_OSX)
	pUniformsBuffer->didModifyRange(NS::Range(0, pUniformsBuffer->length()));
#endif
	
	pComputeEncoder->setBuffer(pUniformsBuffer, 0, BufferIndices::UniformsBuffer);
	pComputeEncoder->setBuffer(pVertexBuffer(), 0, BufferIndices::VertexBuffer);
	pComputeEncoder->setBuffer(pFlippedVertexBuffer(), 0, BufferIndices::FlippedVertexBuffer);
	pComputeEncoder->setBuffer(pIndexBuffer(), 0, BufferIndices::IndexBuffer);
	pComputeEncoder->setBuffer(pInstanceDataBuffer, 0, BufferIndices::InstanceDataBuffer);
	
	pComputeEncoder->useResource(pUniformsBuffer, MTL::ResourceUsageRead);
	pComputeEncoder->useResource(pVertexBuffer(), MTL::ResourceUsageRead);
	pComputeEncoder->useResource(pFlippedVertexBuffer(), MTL::ResourceUsageRead);
	pComputeEncoder->useResource(pIndexBuffer(), MTL::ResourceUsageRead);
	pComputeEncoder->useResource(pInstanceDataBuffer, MTL::ResourceUsageRead);
  }

private:
  MTL::Device * const _pDevice;
  std::vector<VertexData> _vertexData;
  std::vector<VertexData> _flippedVertexData;
  std::vector<uint16_t> _indices;
  const uint16_t _instanceCount;
  
  MTL::Buffer * _pVertexBuffer;
  MTL::Buffer * _pFlippedVertexBuffer;
  MTL::Buffer * _pIndexBuffer;
  std::vector<MTL::Buffer *> _instanceDataBuffers;
  std::vector<MTL::Buffer *> _uniformsBuffers;
  std::unordered_map<uint16_t, InstanceData> _instanceIdToData;
  
  void populateVertexData();
  void loadTextures();
};
