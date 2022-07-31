//
//  Movement.h
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 7/30/22.
//

#ifndef Movement_h
#define Movement_h

#include <cmath>

#include "InputControllerBridge.h"

#include "Transformable.hpp"
#include "Uniforms.hpp"

class Movement : virtual public Transformable {
public:
  Movement();
  virtual ~Movement() = 0;
  
  inline void updateInput(const float_t & deltaTime) {
	const float_t adjustedDeltaTime = deltaTime * 0.5f;
	// Touch location coordinates
	const float_t x = xCoordinate();
	const float_t y = yCoordinate();
	// Start moving if we actually clicked somewhere
	if (x == _defaultCoordinateVal || y == _defaultCoordinateVal) {
	  if (_clippedOffsetX != _defaultCoordinateVal || _clippedOffsetY != _defaultCoordinateVal)
		moveCameraInSameDirection(adjustedDeltaTime);
	} else {
	  moveCameraInNewDirection(adjustedDeltaTime, x, y);
	}
  }

private:
  const float_t _defaultCoordinateVal;
  float_t _clippedOffsetX;
  float_t _clippedOffsetY;
  
  inline void moveCameraInSameDirection(const float_t & adjustedDeltaTime) {
	const float_t moveByX = moveBy(_clippedOffsetX, adjustedDeltaTime);
	const float_t moveByY = moveBy(_clippedOffsetY, adjustedDeltaTime);
	setPosition(position() + glm::vec3(moveByX, moveByY, .0f));
  }
  
  inline const float_t moveBy(float_t & coordinate, const float_t & adjustedDeltaTime) {
	if (coordinate != _defaultCoordinateVal) {
	  float_t moveBy = .0f;
	  if (std::abs(coordinate) < adjustedDeltaTime) {
		moveBy = coordinate;
		coordinate = _defaultCoordinateVal;
	  } else {
		moveBy = coordinate < .0f ? -adjustedDeltaTime : adjustedDeltaTime;
		coordinate = coordinate < .0f ? coordinate + adjustedDeltaTime : coordinate - adjustedDeltaTime;
	  }
	  return moveBy;
	} else {
	  return .0f;
	}
  }
  
  inline void moveCameraInNewDirection(const float_t & adjustedDeltaTime, const float_t & x, const float_t & y) {
	const Math & m = Math::getInstance();
	const Uniforms & uf = Uniforms::getInstance();
	const glm::vec3 targetClipCoords = m.screenToClip(x, y, uf.drawableWidth(), uf.drawableHeight());
	_clippedOffsetX = targetClipCoords.x;
	_clippedOffsetY = targetClipCoords.y;
	setCoordinates(_defaultCoordinateVal, _defaultCoordinateVal);
	moveCameraInSameDirection(adjustedDeltaTime);
  }
};

#endif /* Movement_h */
