//
//  Uniforms.cpp
//  metal-cpp-projects
//
//  Created by Dmitrii Belousov on 7/3/22.
//

#include "Uniforms.hpp"

Uniforms & Uniforms::getInstance()
{
  static Uniforms pInstance;
  return pInstance;
};

Uniforms::Uniforms()
: _modelMatrix(),
  _viewMatrix(),
  _projectionMatrix()
{ };
