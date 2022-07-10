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

const glm::mat4x4 & Uniforms::getModelMatrix() const
{
  return _modelMatrix;
};

void Uniforms::setModelMatrix(const glm::mat4x4 & mMat)
{
  _modelMatrix = mMat;
};

const glm::mat4x4 & Uniforms::getViewMatrix() const
{
  return _viewMatrix;
};

void Uniforms::setViewMatrix(const glm::mat4x4 & vMat)
{
  _viewMatrix = vMat;
};

const glm::mat4x4 & Uniforms::getProjectionMatrix() const
{
  return _projectionMatrix;
};

void Uniforms::setProjectionMatrix(const glm::mat4x4 & pMat)
{
  _projectionMatrix = pMat;
};
