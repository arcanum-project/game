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

#include "Transformable.hpp"
#include "VertexData.hpp"
#include "Uniforms.hpp"
#include "Constants.hpp"

struct InstanceData {
  glm::mat4x4 instanceTransform;
};

class Model : public Transformable
{
public:
  Model(const std::vector<VertexData> vertexData, const std::vector<uint16_t> indices, MTL::Device * const pDevice, const uint16_t instanceCount, const uint16_t maxBuffersInFlight);
  ~Model();
  
  inline const std::vector<uint16_t> & indices() const { return _indices; }
  inline const MTL::Buffer * const vertexBuffer() const { return _pVertexBuffer; }
  inline const MTL::Buffer * const indexBuffer() const { return _pIndexBuffer; }
  inline const std::vector<MTL::Buffer *> instanceDataBuffer() const { return _pInstanceDataBuffer; }
  
  inline void render(MTL::RenderCommandEncoder * const renderEncoder, const uint16_t & frame) const {
	MTL::Buffer * const pInstanceDataBuffer = _pInstanceDataBuffer.at(frame);
	InstanceData * pInstanceData = reinterpret_cast<InstanceData *>(pInstanceDataBuffer->contents());
	for (size_t i = 0; i < RenderingConstants::NumOfTilesPerSector; ++i) {
	  const float_t rowOffset = (float_t) (i % (RenderingConstants::NumOfTilesPerSector / RenderingConstants::NumOfTilesPerRow));
	  const float_t columnOffset = (float_t) (i / (RenderingConstants::NumOfTilesPerSector / RenderingConstants::NumOfTilesPerRow));
	  pInstanceData[i].instanceTransform = Math::getInstance().translation(rowOffset * 2.0f, 0.0f, columnOffset * 2.0f);
	}
	pInstanceDataBuffer->didModifyRange(NS::Range::Make(0, pInstanceDataBuffer->length()));
	
	Uniforms & uf = Uniforms::getInstance();
	uf.setModelMatrix(modelMatrix());
	renderEncoder->setVertexBytes(&uf, sizeof(Uniforms), BufferIndices::UniformsBuffer);
	
	renderEncoder->setVertexBuffer(vertexBuffer(), 0, BufferIndices::VertexBuffer);
	renderEncoder->setVertexBuffer(pInstanceDataBuffer, 0, BufferIndices::InstanceDataBuffer);
	
	_instanceCount > 1
	? renderEncoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, indices().size(), MTL::IndexTypeUInt16, indexBuffer(), 0, _instanceCount)
	: renderEncoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, indices().size(), MTL::IndexTypeUInt16, indexBuffer(), 0);
  }

private:
  const std::vector<VertexData> _vertexData;
  const std::vector<uint16_t> _indices;
  const uint16_t _instanceCount;
  
  MTL::Buffer * const _pVertexBuffer;
  MTL::Buffer * const _pIndexBuffer;
  std::vector<MTL::Buffer *> _pInstanceDataBuffer;
};
