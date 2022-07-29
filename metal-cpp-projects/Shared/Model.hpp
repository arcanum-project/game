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

class Model : public Transformable
{
public:
  Model(const std::vector<VertexData> vertexData, const std::vector<uint16_t> indices, void * const pDevice);
  ~Model();
  
  inline const std::vector<uint16_t> & getIndices() const { return _indices; }
  inline const MTL::Buffer * const getVertexBuffer() const { return _pVertexBuffer; }
  inline const MTL::Buffer * const getIndexBuffer() const { return _pIndexBuffer; }
  
  inline void render(MTL::RenderCommandEncoder * const renderEncoder) const {
	Uniforms & uf = Uniforms::getInstance();
	uf.setModelMatrix(modelMatrix());
	renderEncoder->setVertexBytes(&uf, sizeof(Uniforms), BufferIndices::UniformsBuffer);
	
	renderEncoder->setVertexBuffer(getVertexBuffer(), 0, BufferIndices::VertexBuffer);
	
	renderEncoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, getIndices().size(), MTL::IndexTypeUInt16, getIndexBuffer(), 0);
  }

private:
  const std::vector<VertexData> _vertexData;
  const std::vector<uint16_t> _indices;
  
  MTL::Buffer * const _pVertexBuffer;
  MTL::Buffer * const _pIndexBuffer;
};
