//
//  Model.hpp
//  fragments
//
//  Created by Dmitrii Belousov on 7/10/22.
//

#pragma once

#include "Metal/Metal.hpp"
#include <vector>
#include "glm/vec3.hpp"
#include "TargetConditionals.h"

#include "Transformable.hpp"
#include "VertexData.hpp"
#include "Uniforms.hpp"
#include "Constants.hpp"
#include "MemoryAlignment.hpp"

struct InstanceData {
  glm::mat4x4 instanceTransform;
};

class Model : public Transformable
{
public:
  Model(const std::vector<VertexData> vertexData, const std::vector<uint16_t> indices, MTL::Device * const pDevice, const uint16_t instanceCount, const uint16_t maxBuffersInFlight, const char * textureImgName, const char * textureImgType);
  ~Model();
  
  inline const std::vector<uint16_t> & indices() const { return _indices; }
  inline const MTL::Buffer * const vertexBuffer() const { return _pVertexBuffer; }
  inline const MTL::Buffer * const indexBuffer() const { return _pIndexBuffer; }
  inline const std::vector<MTL::Buffer *> instanceDataBuffer() const { return _pInstanceDataBuffer; }
  inline const std::vector<MTL::Buffer *> uniformsBuffer() const { return _pUniformsBuffer; }
  
  inline void render(MTL::ComputeCommandEncoder * const pComputeEncoder, const uint16_t & frame) const {
	MTL::Buffer * const pInstanceDataBuffer = _pInstanceDataBuffer.at(frame);
	InstanceData * pInstanceData = reinterpret_cast<InstanceData *>(pInstanceDataBuffer->contents());
	for (size_t i = 0; i < RenderingConstants::NumOfTilesPerSector; ++i) {
	  const float_t rowOffset = (float_t) (i % (RenderingConstants::NumOfTilesPerSector / RenderingConstants::NumOfTilesPerRow));
	  const float_t columnOffset = (float_t) (i / (RenderingConstants::NumOfTilesPerSector / RenderingConstants::NumOfTilesPerRow));
	  pInstanceData[i].instanceTransform = Math::getInstance().translation(rowOffset * 2.0f, 0.0f, columnOffset * 2.0f);
	}
#if defined(TARGET_OSX)
	pInstanceDataBuffer->didModifyRange(NS::Range::Make(0, pInstanceDataBuffer->length()));
#endif
	
	Uniforms & uf = Uniforms::getInstance();
	uf.setModelMatrix(modelMatrix());
	MTL::Buffer * const pUniformsBuffer = _pUniformsBuffer.at(frame);
	memcpy(pUniformsBuffer->contents(), & uf, MemoryAlignment::roundUpToNextMultipleOf16(sizeof(Uniforms)));
#if defined(TARGET_OSX)
	pUniformsBuffer->didModifyRange(NS::Range(0, pUniformsBuffer->length()));
#endif
	
	pComputeEncoder->setBuffer(pUniformsBuffer, 0, BufferIndices::UniformsBuffer);
	pComputeEncoder->setBuffer(vertexBuffer(), 0, BufferIndices::VertexBuffer);
	pComputeEncoder->setBuffer(indexBuffer(), 0, BufferIndices::IndexBuffer);
	pComputeEncoder->setBuffer(pInstanceDataBuffer, 0, BufferIndices::InstanceDataBuffer);
	
	pComputeEncoder->useResource(pUniformsBuffer, MTL::ResourceUsageRead);
	pComputeEncoder->useResource(vertexBuffer(), MTL::ResourceUsageRead);
	pComputeEncoder->useResource(indexBuffer(), MTL::ResourceUsageRead);
	pComputeEncoder->useResource(pInstanceDataBuffer, MTL::ResourceUsageRead);
  }

private:
  MTL::Device * const _pDevice;
  const std::vector<VertexData> _vertexData;
  const std::vector<uint16_t> _indices;
  const uint16_t _instanceCount;
  
  MTL::Buffer * const _pVertexBuffer;
  MTL::Buffer * const _pIndexBuffer;
  std::vector<MTL::Buffer *> _pInstanceDataBuffer;
  std::vector<MTL::Buffer *> _pUniformsBuffer;
  MTL::Texture * const _pTexture;
};
