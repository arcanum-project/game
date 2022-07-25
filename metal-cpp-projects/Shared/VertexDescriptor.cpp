//
//  VertexDescriptor.cpp
//  triangle-with-animation
//
//  Created by Dmitrii Belousov on 7/2/22.
//

#include "VertexDescriptor.hpp"
#include "Constants.hpp"

const VertexDescriptor & VertexDescriptor::getInstance()
{
  static VertexDescriptor pInstance;
  return pInstance;
};

VertexDescriptor::VertexDescriptor()
: _default()
{
  //  Default layout
  _default = MTL::VertexDescriptor::alloc()->init();
  
  // Position
  _default->attributes()->object(Attributes::VertexCoordinates)->setFormat(MTL::VertexFormatFloat3);
  _default->attributes()->object(Attributes::VertexCoordinates)->setOffset(0);
  _default->attributes()->object(Attributes::VertexCoordinates)->setBufferIndex(BufferIndices::VertexBuffer);
  size_t offset = sizeof(float_t) * 3;
  // Texture
  _default->attributes()->object(Attributes::TextureCoordinates)->setFormat(MTL::VertexFormatFloat2);
  _default->attributes()->object(Attributes::TextureCoordinates)->setOffset(offset);
  _default->attributes()->object(Attributes::TextureCoordinates)->setBufferIndex(BufferIndices::VertexBuffer);
  offset += sizeof(float_t) * 2;
  // Normal
  _default->attributes()->object(Attributes::NormalCoordinates)->setFormat(MTL::VertexFormatFloat3);
  _default->attributes()->object(Attributes::NormalCoordinates)->setOffset(offset);
  _default->attributes()->object(Attributes::NormalCoordinates)->setBufferIndex(BufferIndices::VertexBuffer);
  offset += sizeof(float_t) * 3;
  
  _default->layouts()->object(0)->setStride(offset);
};

VertexDescriptor::~VertexDescriptor()
{
  _default->release();
};
