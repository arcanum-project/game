//
//  IsometricCamera.cpp
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 7/24/22.
//

#include "IsometricCamera.hpp"

IsometricCamera::IsometricCamera()
: _near(0.1f),
  _far(100.0f),
  _drawableWidth(1.0f),
  _drawableHeight(1.0f) {}
