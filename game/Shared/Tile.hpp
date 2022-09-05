//
//  Tile.hpp
//  iOS
//
//  Created by Dmitrii Belousov on 9/2/22.
//

#ifndef Tile_hpp
#define Tile_hpp

#include <Metal/Metal.hpp>
#include <vector>
#include <TargetConditionals.h>
#include <unordered_map>
#include <string>

#include "Uniforms.hpp"
#include "Model.hpp"
#include "VertexData.hpp"
#include "Constants.h"
#include "Common/Alignment.hpp"
#include "InstanceData.hpp"

class Tile : public Model
{
public:
  Tile(MTL::Device * const pDevice, const uint16_t instanceCount, const uint16_t maxBuffersInFlight);
  ~Tile();
  
  inline const MTL::Buffer * const pFlippedVertexBuffer() const { return _pFlippedVertexBuffer; }
  inline const std::vector<MTL::Buffer *> instanceDataBuffers() const { return _instanceDataBuffers; }
  
  inline void render(MTL::CommandEncoder * const pCommandEncoder, const uint16_t & frame) override {
	MTL::ComputeCommandEncoder * const pComputeEncoder = reinterpret_cast<MTL::ComputeCommandEncoder * const>(pCommandEncoder);
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
	MTL::Buffer * const pUniformsBuffer = uniformsBuffers().at(frame);
	memcpy(pUniformsBuffer->contents(), & uf, Alignment::roundUpToNextMultipleOf16(sizeof(Uniforms)));
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
  std::vector<VertexData> _flippedVertexData;
  const uint16_t _instanceCount;
  MTL::Buffer * _pFlippedVertexBuffer;
  std::vector<MTL::Buffer *> _instanceDataBuffers;
  std::unordered_map<uint16_t, InstanceData> _instanceIdToData;
  
  void populateVertexData() override;
  void loadTextures() override;
};

#endif /* Tile_hpp */
