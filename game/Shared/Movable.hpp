//
//  Movable.h
//
//  Created by Dmitrii Belousov on 7/30/22.
//

#pragma once

#include "InputControllerBridge.h"
#include "Transformable.hpp"
#include "Uniforms.hpp"
#include "GameSettings.h"

class Movable {
public:
  Movable();
  virtual ~Movable() = 0;
  
  bool move(glm::vec3& outPositionWorld, const glm::vec3& currentPositionWorld, const float_t speed);
  unsigned char getDirectionIndex(const glm::vec3& currentPositionWorld) const;

private:
  const float_t _defaultCoordinateVal;
  glm::vec4 targetPositionWorld;
  
  inline glm::vec3 getDirectionVectorWorld(const glm::vec3& currentPositionWorld) const
  {
	if (targetPositionWorld.x == 0.f && targetPositionWorld.y == 0.f && targetPositionWorld.z == 0.f) return std::move(glm::vec3());
	// Based on: https://math.stackexchange.com/questions/3932112/move-a-point-along-a-vector-by-a-given-distance
	const glm::vec3 directionVectorWorld = glm::vec3(targetPositionWorld.x - currentPositionWorld.x, 0.f, targetPositionWorld.z - currentPositionWorld.z);
	return directionVectorWorld;
  }
  
  inline const bool isTargetPositionSet() const
  {
	return targetPositionWorld.x != 0.f || targetPositionWorld.y != 0.f || targetPositionWorld.z != 0.f || targetPositionWorld.w != 0.f;
  }
  
  virtual bool moveInSameDirection(glm::vec3& outPositionWorld, const glm::vec3& currentPositionWorld, const float_t speed) const;
  
  bool moveInNewDirection(glm::vec3& outPositionWorld, const glm::vec3& currentPositionWorld, const float_t speed, const float_t xScreen, const float_t yScreen);
};
