//
//  Math.hpp
//  cameras
//
//  Created by Dmitrii Belousov on 7/3/22.
//

#pragma once

#include "glm/mat4x4.hpp"
#include "glm/vec4.hpp"
#include "glm/trigonometric.hpp"

class Math {
public:
  static const Math & getInstance();
  Math(Math const &) = delete;
  void operator=(Math const &) = delete;
  
  inline const glm::mat4x4 translation(const float_t & trX, const float_t & trY, const float_t & trZ) const {
	glm::mat4x4 mat = _identity;
	mat[3].x = trX;
	mat[3].y = trY;
	mat[3].z = trZ;
	return mat;
  }
  
  inline const glm::mat4x4 translation(const glm::vec3 & tr) const {
	glm::mat4x4 mat = _identity;
	mat[3].x = tr.x;
	mat[3].y = tr.y;
	mat[3].z = tr.z;
	return mat;
  }
  
  inline const glm::mat4x4 scaling(const float_t & scaleX, const float_t & scaleY, const float_t & scaleZ) const {
	glm::mat4x4 mat = _identity;
	mat[0].x = scaleX;
	mat[1].y = scaleY;
	mat[2].z = scaleZ;
	return mat;
  }
  
  inline const glm::mat4x4 scaling(const float_t & scale) const {
	glm::mat4x4 mat = _identity;
	mat[3].w = 1 / scale;
	return mat;
  }

  inline const glm::mat4x4 rotationX(const float_t & angleDegrees) const {
//	  float_t angleRad = glm::radians(angleDegrees);
	const float_t angleRad = angleDegrees;
	glm::mat4x4 mat = _identity;
	mat[1].y = cosf(angleRad);
	mat[1].z = sinf(angleRad);
	mat[2].y = -sinf(angleRad);
	mat[2].z = cosf(angleRad);
	return mat;
  }
  
  inline const glm::mat4x4 rotationY(const float_t & angleDegrees) const {
//	  float_t angleRad = glm::radians(angleDegrees);
	const float_t angleRad = angleDegrees;
	glm::mat4x4 mat = _identity;
	mat[0].x = cosf(angleRad);
	mat[0].z = -sinf(angleRad);
	mat[2].x = sinf(angleRad);
	mat[2].z = cosf(angleRad);
	return mat;
  }

  inline const glm::mat4x4 rotationZ(const float_t & angleDegrees) const {
//	  float_t angleRad = glm::radians(angleDegrees);
	const float_t angleRad = angleDegrees;
	glm::mat4x4 mat = _identity;
	mat[0].x = cosf(angleRad);
	mat[0].y = sinf(angleRad);
	mat[1].x = -sinf(angleRad);
	mat[1].y = cosf(angleRad);
	return mat;
  }

  inline const glm::mat4x4 rotation(const float_t & angleDegrees) const {
	return rotationX(angleDegrees) * rotationY(angleDegrees) * rotationZ(angleDegrees);
  }
  
  inline const glm::mat4x4 rotation(const glm::vec3 & rotationDegrees) const {
	return rotationX(rotationDegrees.x) * rotationY(rotationDegrees.y) * rotationZ(rotationDegrees.z);
  }
  
  inline const glm::mat4x4 rotationYXZ(const glm::vec3 & rotationDegrees) const {
	return rotationY(rotationDegrees.y) * rotationX(rotationDegrees.x) * rotationZ(rotationDegrees.z);
  }
  
  inline const glm::mat4x4 perspective(const float_t & fovDegrees, const float_t & near, const float_t & far, const float_t & aspect) const {
	float_t fovRad = glm::radians(fovDegrees);
	glm::mat4x4 mat = _identity;
	// Left-handed coordinate system
	mat[1].y = 1 / tanf(fovRad * 0.5f);
	mat[0].x = mat[1].y / aspect;
	mat[2].z = far / (far  - near);
	mat[2].w = 1;
	mat[3].z = mat[2].z * -near;
	mat[3].w = 0;
	return mat;
  }

  inline const glm::mat4x4 isometric(const float_t & drawableWidth, const float_t & drawableHeight, const float_t & near, const float_t & far) const {
	// Based on: https://stackoverflow.com/questions/54827687/normalized-device-coordinate-metal-coming-from-opengl
	glm::mat4x4 mat = _identity;
	mat[0].x = drawableHeight / drawableWidth;
	mat[2].z = 1.0f / (far - near);
	mat[3].z = near / (near - far);
	return mat;
  }
  
  /**
   Convert X, Y coordinates from screen space to NDC space.
   Based on: https://stackoverflow.com/questions/54827687/normalized-device-coordinate-metal-coming-from-opengl
   */
  inline const glm::vec3 screenToNDC(const float_t & x, const float_t & y, const float_t & screenWidth, const float_t & screenHeight) const {
	glm::vec4 vec = glm::vec4(x, y, .0f, 1.0f);
	const float_t scaleX = 2.0f / screenWidth;
	const float_t scaleY = -2.0f / screenHeight;
	const glm::mat4x4 scaleMat = glm::mat4x4({ scaleX, .0f, .0f, .0f }, { .0f, scaleY, .0f, .0f }, { .0f, .0f, .0f, .0f }, { -1.0f, 1.0f, .0f, .0f });
	glm::vec3 ndc = glm::vec3(scaleMat * vec);
	ndc.z = .0f;
	return ndc;
  }

private:
  Math();
  const glm::mat4x4 _identity;
  inline const glm::mat4x4 identity() const { return _identity; }
};
