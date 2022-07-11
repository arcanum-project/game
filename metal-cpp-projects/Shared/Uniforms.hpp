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
  
	inline const glm::mat4x4 & getModelMatrix() const { return  _modelMatrix; }
	inline void setModelMatrix(const glm::mat4x4 & mMat) { _modelMatrix = mMat; }
	inline const glm::mat4x4 & getViewMatrix() const { return _viewMatrix; }
	inline void setViewMatrix(const glm::mat4x4 & vMat) { _viewMatrix = vMat; }
	inline const glm::mat4x4 & getProjectionMatrix() const { return _projectionMatrix; }
	inline void setProjectionMatrix(const glm::mat4x4 & pMat) { _projectionMatrix = pMat; }
  
  private:
	glm::mat4x4 _modelMatrix;
	glm::mat4x4 _viewMatrix;
	glm::mat4x4 _projectionMatrix;
	Uniforms();
};
