//
//  Model.hpp
//  fragments
//
//  Created by Dmitrii Belousov on 7/10/22.
//

#pragma once

#include <Metal/Metal.hpp>
#include <vector>

#include "Transformable.hpp"
#include "VertexData.hpp"

class Model : public Transformable {
public:
  Model(MTL::Device * const pDevice, const uint16_t maxBuffersInFlight);
  virtual ~Model();
  
  inline MTL::Device * const pDevice() const { return _pDevice; }
  inline const std::vector<VertexData> & vertexData() const { return _vertexData; }
  inline void setVertexData(const std::vector<VertexData> vertexData) { _vertexData = vertexData; }
  inline const std::vector<uint16_t> & indices() const { return _indices; }
  inline void setIndices(const std::vector<uint16_t> indices) { _indices = indices; }
  inline MTL::Buffer * const pVertexBuffer() const { return _pVertexBuffer; }
  inline void setVertexBuffer(MTL::Buffer * const pVertexBuffer) { _pVertexBuffer = pVertexBuffer; }
  inline MTL::Buffer * const pIndexBuffer() const { return _pIndexBuffer; }
  inline void setIndexBuffer(MTL::Buffer * const pIndexBuffer) { _pIndexBuffer = pIndexBuffer; }
  inline const std::vector<MTL::Buffer *> uniformsBuffers() const { return _uniformsBuffers; }
  
  virtual void populateVertexData() = 0;
  virtual void loadTextures() = 0;
  virtual void update(float_t deltaTime) = 0;

private:
  MTL::Device * const _pDevice;
  std::vector<VertexData> _vertexData;
  std::vector<uint16_t> _indices;
  MTL::Buffer * _pVertexBuffer;
  MTL::Buffer * _pIndexBuffer;
  std::vector<MTL::Buffer *> _uniformsBuffers;
};
