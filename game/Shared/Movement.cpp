//
//  Movement.cpp
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 7/30/22.
//

#include "Movement.hpp"

Movement::Movement()
: _defaultCoordinateVal(.0f),
  targetPositionWorld()
{}

Movement::~Movement() {};

bool Movement::move(glm::vec3& outPositionWorld, const float_t speed, const glm::vec3& currentPositionWorld) {
  // Touch location coordinates
  const float_t xScreen = xCoordinate();
  const float_t yScreen = yCoordinate();
  // Start moving if we actually clicked somewhere
  if (xScreen == _defaultCoordinateVal || yScreen == _defaultCoordinateVal) {
	if (!isTargetPositionSet()) return false;
	const glm::vec3 cmpCurrTarget = glm::equal(currentPositionWorld, std::move(glm::vec3(targetPositionWorld.x, targetPositionWorld.y, targetPositionWorld.z)));
	if (cmpCurrTarget.x && cmpCurrTarget.y && cmpCurrTarget.z) return false;
	return moveInSameDirection(outPositionWorld, currentPositionWorld, speed);
  }
  else return moveInNewDirection(outPositionWorld, currentPositionWorld, speed, xScreen, yScreen);
}

bool Movement::moveInSameDirection(glm::vec3 &outPosition, const glm::vec3& currentPositionWorld, const float_t speed) const {
  if (std::abs(targetPositionWorld.x - currentPositionWorld.x) <= speed && std::abs(targetPositionWorld.z - currentPositionWorld.z) <= speed)
  {
	outPosition = glm::vec3(targetPositionWorld);
  }
  else
  {
	// Based on: https://math.stackexchange.com/questions/3932112/move-a-point-along-a-vector-by-a-given-distance
	const glm::vec3 directionVector = glm::vec3(targetPositionWorld.x - currentPositionWorld.x, 0.f, targetPositionWorld.z - currentPositionWorld.z);
	outPosition = currentPositionWorld + speed * directionVector / glm::length(directionVector);
  }
  return true;
}

bool Movement::moveInNewDirection(glm::vec3& outPosition, const glm::vec3& currentPositionWorld, const float_t speed, const float_t xScreen, const float_t yScreen) {
  const Math& m = Math::getInstance();
  const Uniforms& uf = Uniforms::getInstance();
  const glm::vec4 newPositionWorld = m.screenToWorld(xScreen, yScreen, uf.drawableWidth(), uf.drawableHeight(), uf.getViewMatrix(), uf.getProjectionMatrix());
  targetPositionWorld = std::move(newPositionWorld);
  return moveInSameDirection(outPosition, currentPositionWorld, speed);
}
