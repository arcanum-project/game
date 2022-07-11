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

inline const glm::mat4x4 & Uniforms::getModelMatrix() const
{
  return _modelMatrix;
};

inline void Uniforms::setModelMatrix(const glm::mat4x4 & mMat)
{
  _modelMatrix = mMat;
};

inline const glm::mat4x4 & Uniforms::getViewMatrix() const
{
  return _viewMatrix;
};

inline void Uniforms::setViewMatrix(const glm::mat4x4 & vMat)
{
  _viewMatrix = vMat;
};

inline const glm::mat4x4 & Uniforms::getProjectionMatrix() const
{
  return _projectionMatrix;
};

inline void Uniforms::setProjectionMatrix(const glm::mat4x4 & pMat)
{
  _projectionMatrix = pMat;
};
