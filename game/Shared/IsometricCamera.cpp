//
//  IsometricCamera.cpp
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 7/24/22.
//

#include "IsometricCamera.hpp"
#include "Uniforms.hpp"

IsometricCamera::IsometricCamera()
: _near(0.5f),
  _far(100.0f),
  _drawableWidth(Uniforms::getInstance().drawableWidth()),
  _drawableHeight(Uniforms::getInstance().drawableHeight()) {}
