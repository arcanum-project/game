//
//  Uniforms.hpp
//  cameras
//
//  Created by Dmitrii Belousov on 7/3/22.
//

#pragma once

#include "glm/mat4x4.hpp"

class Uniforms
{
  public:
	static Uniforms & getInstance();
	// Tell compielr not to generate default copy constructors
	// From here: https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
	Uniforms(Uniforms const &) = delete;
	void operator=(Uniforms const &) = delete;
  
	const glm::mat4x4 & getModelMatrix() const;
	void setModelMatrix(const glm::mat4x4 & mMat);
	const glm::mat4x4 & getViewMatrix() const;
	void setViewMatrix(const glm::mat4x4 & vMat);
	const glm::mat4x4 & getProjectionMatrix() const;
	void setProjectionMatrix(const glm::mat4x4 & pMat);
  
  
  private:
	glm::mat4x4 _modelMatrix;
	glm::mat4x4 _viewMatrix;
	glm::mat4x4 _projectionMatrix;
	Uniforms();
};
