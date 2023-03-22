//
//  Movement.cpp
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 7/30/22.
//

#include <utility>
#include <cmath>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "Movement.hpp"
#include "GameSettings.h"

Movement::Movement()
: _defaultCoordinateVal(.0f),
  targetPositionWorld()
{}

Movement::~Movement() {};

bool Movement::move(glm::vec3& outPositionWorld, const glm::vec3& currentPositionWorld, const float_t speed) {
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

bool Movement::moveInSameDirection(glm::vec3& outPositionWorld, const glm::vec3& currentPositionWorld, const float_t speed) const {
  if (std::abs(targetPositionWorld.x - currentPositionWorld.x) <= speed && std::abs(targetPositionWorld.z - currentPositionWorld.z) <= speed)
  {
	outPositionWorld = glm::vec3(targetPositionWorld);
  }
  else
  {
	glm::vec3 directionVectorWorld = getDirectionVectorWorld(currentPositionWorld);
	outPositionWorld = currentPositionWorld + speed * directionVectorWorld / glm::length(directionVectorWorld);
  }
  return true;
}

bool Movement::moveInNewDirection(glm::vec3& outPositionWorld, const glm::vec3& currentPositionWorld, const float_t speed, const float_t xScreen, const float_t yScreen) {
  const Math& m = Math::getInstance();
  const Uniforms& uf = Uniforms::getInstance();
  const glm::vec4 newPositionWorld = m.screenToWorld(xScreen, yScreen, uf.drawableWidth(), uf.drawableHeight(), uf.getViewMatrix(), uf.getProjectionMatrix());
  targetPositionWorld = std::move(newPositionWorld);
  return moveInSameDirection(outPositionWorld, currentPositionWorld, speed);
}

unsigned char Movement::getDirectionIndex(const glm::vec3& currentPositionWorld) const
{
  const Math& m = Math::getInstance();
  const Uniforms& uf = Uniforms::getInstance();
  glm::vec3 directionvectorWorld = getDirectionVectorWorld(currentPositionWorld);
  const glm::vec2 directionVectorNDC = m.worldToNDC(std::move(glm::vec4(directionvectorWorld, 0.f)), uf.getViewMatrix(), uf.getProjectionMatrix());
  
  if (directionVectorNDC.x > RenderingSettings::DirectionEpsilonNDC)
  {
	if (directionVectorNDC.y > RenderingSettings::DirectionEpsilonNDC) return 1;
	if (directionVectorNDC.y >= -RenderingSettings::DirectionEpsilonNDC && directionVectorNDC.y <= RenderingSettings::DirectionEpsilonNDC) return 2;
	if (directionVectorNDC.y < -RenderingSettings::DirectionEpsilonNDC) return 3;
  }
  if (directionVectorNDC.x < -RenderingSettings::DirectionEpsilonNDC)
  {
	if (directionVectorNDC.y > RenderingSettings::DirectionEpsilonNDC) return 7;
	if (directionVectorNDC.y >= -RenderingSettings::DirectionEpsilonNDC && directionVectorNDC.y <= RenderingSettings::DirectionEpsilonNDC) return 6;
	if (directionVectorNDC.y < -RenderingSettings::DirectionEpsilonNDC) return 5;
  }
  if (directionVectorNDC.x >= -RenderingSettings::DirectionEpsilonNDC && directionVectorNDC.x <= RenderingSettings::DirectionEpsilonNDC)
  {
	if (directionVectorNDC.y >= 0) return 0;
	else return 4;
  }
  
  return 0;
}
