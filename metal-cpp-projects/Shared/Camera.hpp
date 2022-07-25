//
//  Camera.hpp
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 7/24/22.
//

#ifndef Camera_hpp
#define Camera_hpp

#include <stdio.h>
#include "Transformable.hpp"
#include "glm/mat4x4.hpp"

class Camera : public Transformable {
public:
  virtual ~Camera() = 0;
  
  virtual const glm::mat4x4 viewMatrix() = 0;
  virtual const glm::mat4x4 projectionMatrix() = 0;
  virtual void update(const float_t & drawableWidth, const float_t & drawableHeight) = 0;
  virtual void update(const float_t & deltaTime) = 0;
};

#endif /* Camera_hpp */
