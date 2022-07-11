//
//  Transformable.cpp
//  cameras
//
//  Created by Dmitrii Belousov on 7/9/22.
//

#include "Transformable.hpp"

Transformable::Transformable()
: _position(glm::vec3(0, 0, 0)),
  _rotation(glm::vec3(0, 0, 0)),
  _scale(1)
{ }

const glm::vec3 & Transformable::getPosition() const
{
  return _position;
}

void Transformable::setPosition(const glm::vec3 & position)
{
  _position = position;
}

const glm::vec3 & Transformable::getRotation() const
{
  return _rotation;
}

void Transformable::setRotation(const glm::vec3 & rotation)
{
  _rotation = rotation;
}

const float_t & Transformable::getScale() const
{
  return _scale;
}

void Transformable::setScale(const float_t & scale)
{
  _scale = scale;
}

const glm::mat4x4 Transformable::getModelMatrix() const
{
  const Math & m = Math::getInstance();
  glm::mat4x4 trMat = m.translation(_position);
  glm::mat4x4 rotMat = m.rotation(_rotation);
  glm::mat4x4 scaleMat = m.scaling(_scale);
  return trMat * rotMat * scaleMat;
}
