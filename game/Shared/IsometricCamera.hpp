//
//  IsometricCamera.hpp
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 7/24/22.
//

#ifndef IsometricCamera_hpp
#define IsometricCamera_hpp

#include <stdio.h>

#include "Camera.hpp"
#include "Math.hpp"
#include "Movement.hpp"

class IsometricCamera : public Camera, public Movement {
public:
  IsometricCamera();
  
  inline const glm::mat4x4 viewMatrix() override {
	const Math & m = Math::getInstance();
	return glm::inverse(m.translation(position()) * m.rotationYXZ(rotation()) * m.scaling(scale()));
  }
  inline const glm::mat4x4 projectionMatrix() override
  {
	return Math::getInstance().isometric(_drawableWidth, _drawableHeight, _near, _far);
  }
  inline void update(const float_t & drawableWidth, const float_t & drawableHeight) override {
	_drawableWidth = drawableWidth;
	_drawableHeight = drawableHeight;
  }
  inline void update(const float_t & deltaTime) override {
	glm::vec3 outPositionWorld;
	if (move(outPositionWorld, position(), deltaTime * GameplaySettings::CameraMovementSpeed))
	  setPosition(outPositionWorld);
  }

private:
  float_t _drawableWidth;
  float_t _drawableHeight;
  float_t _near;
  float_t _far;
};

#endif /* IsometricCamera_hpp */
