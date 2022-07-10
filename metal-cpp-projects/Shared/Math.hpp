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

class Math
{
  public:
	static const Math & getInstance();
	Math(Math const &) = delete;
	void operator=(Math const &) = delete;
	const glm::mat4x4 translation(const float_t & trX, const float_t & trY, const float_t & trZ) const;
	const glm::mat4x4 translation(const glm::vec3 & tr) const;
	const glm::mat4x4 scaling(const float_t & scaleX, const float_t & scaleY, const float_t & scaleZ) const;
	const glm::mat4x4 scaling(const float_t & scale) const;
	const glm::mat4x4 rotationX(const float_t & angleDegrees) const;
	const glm::mat4x4 rotationY(const float_t & angleDegrees) const;
	const glm::mat4x4 rotationZ(const float_t & angleDegrees) const;
	const glm::mat4x4 rotation(const float_t & angleDegrees) const;
	const glm::mat4x4 rotation(const glm::vec3 & rotationDegrees) const;
	const glm::mat4x4 perspective(const float_t & fovDegrees, const float_t & near, const float_t & far, const float_t & aspect) const;
  private:
	Math();
	const glm::mat4x4 _identity;
	const glm::mat4x4 identity() const;
};
