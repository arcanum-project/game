//
//  Movement.h
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 7/30/22.
//

#pragma once

#include "InputControllerBridge.h"
#include "Transformable.hpp"
#include "Uniforms.hpp"
#include "GameSettings.hpp"

struct PositionWorld
{
  glm::vec3 position;
  unsigned char directionIndex;
};

class Movement {
public:
  Movement();
  virtual ~Movement() = 0;
  
  bool move(PositionWorld& outPositionWorld, const float_t speed, const glm::vec3& currentPositionWorld, const bool bCalculateDirection = false);

private:
  const float_t _defaultCoordinateVal;
  glm::vec4 targetPositionWorld;
  
  inline const bool isTargetPositionSet() const
  {
	return targetPositionWorld.x != 0.f || targetPositionWorld.y != 0.f || targetPositionWorld.z != 0.f || targetPositionWorld.w != 0.f;
  }
  
  bool moveInSameDirection(PositionWorld& outPositionWorld, const glm::vec3& currentPositionWorld, const float_t speed, const bool bCalculateDirection) const;
  
  bool moveInNewDirection(PositionWorld& outPositionWorld, const glm::vec3& currentPositionWorld, const float_t speed, const float_t xScreen, const float_t yScreen, const bool bCalculateDirection);
  
  unsigned char calculateDirectionIndex(const glm::vec3& directionVectorWorld) const;
};
