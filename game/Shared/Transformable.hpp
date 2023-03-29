//
//  Transformable.hpp
//  cameras
//
//  Created by Dmitrii Belousov on 7/9/22.
//

#pragma once

#include "Math.hpp"

class Transformable
{
  public:
	// Constructor
	Transformable();
	// Destructor
	virtual ~Transformable() = 0;
	
	// Methods
	inline const glm::vec3& position() const { return _position; }
	inline void setPosition(const glm::vec3& position) { _position = position; }
	inline const glm::vec3& rotation() const { return _rotation; }
	inline void setRotation(const glm::vec3& rotation) { _rotation = rotation; }
	inline const float_t scale() const { return _scale; }
	inline void setScale(const float_t scale) { _scale = scale; }
  
	const glm::mat4x4 modelMatrix() const;
  
  private:
	glm::vec3 _position;
	glm::vec3 _rotation;
	float_t _scale;
	glm::mat4x4 _modelMatrix;
};
