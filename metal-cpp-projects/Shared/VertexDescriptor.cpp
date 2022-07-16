//
//  VertexDescriptor.cpp
//  triangle-with-animation
//
//  Created by Dmitrii Belousov on 7/2/22.
//

#include "VertexDescriptor.hpp"

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
  _default->attributes()->object(0)->setFormat(MTL::VertexFormatFloat3);
  _default->attributes()->object(0)->setOffset(0);
  _default->attributes()->object(0)->setBufferIndex(0);
  size_t offset = sizeof(float_t) * 3;
  // Texture
  _default->attributes()->object(1)->setFormat(MTL::VertexFormatFloat2);
  _default->attributes()->object(1)->setOffset(offset);
  _default->attributes()->object(1)->setBufferIndex(0);
  offset += sizeof(float_t) * 2;
  // Normal
  _default->attributes()->object(2)->setFormat(MTL::VertexFormatFloat3);
  _default->attributes()->object(2)->setOffset(offset);
  _default->attributes()->object(2)->setBufferIndex(0);
  offset += sizeof(float_t) * 3;
  _default->layouts()->object(0)->setStride(offset);
};

VertexDescriptor::~VertexDescriptor()
{
  _default->release();
};
