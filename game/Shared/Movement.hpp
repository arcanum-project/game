//
//  Movement.h
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 7/30/22.
//

#ifndef Movement_h
#define Movement_h

#include <cmath>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <utility>

#include "InputControllerBridge.h"
#include "Transformable.hpp"
#include "Uniforms.hpp"
#include "GameSettings.hpp"

class Movement {
public:
  Movement();
  virtual ~Movement() = 0;
  
  bool move(glm::vec3& outPositionWorld, const float_t speed, const glm::vec3& currentPositionWorld);

private:
  const float_t _defaultCoordinateVal;
  glm::vec4 targetPositionWorld;
  
  inline const bool isTargetPositionSet() const
  {
	return targetPositionWorld.x != 0.f || targetPositionWorld.y != 0.f || targetPositionWorld.z != 0.f || targetPositionWorld.w != 0.f;
  }
  
  bool moveInSameDirection(glm::vec3 &outPosition, const glm::vec3& currentPositionWorld, const float_t speed) const;
  
  bool moveInNewDirection(glm::vec3& outPosition, const glm::vec3& currentPositionWorld, const float_t speed, const float_t xScreen, const float_t yScreen);
};

#endif /* Movement_h */
