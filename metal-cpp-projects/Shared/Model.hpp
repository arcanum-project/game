//
//  Model.hpp
//  fragments
//
//  Created by Dmitrii Belousov on 7/10/22.
//

#pragma once

#include "Metal/Metal.hpp"
#include "glm/vec3.hpp"
#include "Transformable.hpp"
#include <vector>
#include <array>

class Model : public Transformable
{
  public:
	Model(const std::vector<std::array<glm::vec3, 2>> & vertexData, const std::vector<uint16_t> & indices, MTL::Device * const pDevice);
	~Model();
	
	inline const std::vector<uint16_t> & getIndices() const { return _indices; }
	inline const MTL::Buffer * const getVertexBuffer() const { return _pVertexBuffer; }
	inline const MTL::Buffer * const getIndexBuffer() const { return _pIndexBuffer; }
  
  private:
	const std::vector<std::array<glm::vec3, 2>> _vertexData;
	const std::vector<uint16_t> _indices;
	
	MTL::Buffer * const _pVertexBuffer;
	MTL::Buffer * const _pIndexBuffer;
};
