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

Transformable::~Transformable() { }

const glm::mat4x4 Transformable::modelMatrix() const
{
  const Math & m = Math::getInstance();
  glm::mat4x4 trMat = m.translation(_position);
  glm::mat4x4 rotMat = m.rotation(_rotation);
  glm::mat4x4 scaleMat = m.scaling(_scale);
  return trMat * rotMat * scaleMat;
}
