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

const MTL::VertexDescriptor * const VertexDescriptor::getDefaultLayout() const
{
  return _default;
};

VertexDescriptor::VertexDescriptor()
{
  //  Default layout
  _default = MTL::VertexDescriptor::alloc()->init();
  
  // Position
  _default->attributes()->object(0)->setFormat(MTL::VertexFormatFloat3);
  _default->attributes()->object(0)->setOffset(0);
  _default->attributes()->object(0)->setBufferIndex(0);
  _default->layouts()->object(0)->setStride(sizeof(float_t) * 3);
  
  // Color
  _default->attributes()->object(1)->setFormat(MTL::VertexFormatFloat3);
  _default->attributes()->object(1)->setOffset(0);
  _default->attributes()->object(1)->setBufferIndex(1);
  _default->layouts()->object(1)->setStride(sizeof(glm::vec3));
};

VertexDescriptor::~VertexDescriptor()
{
  _default->release();
};
