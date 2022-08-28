//
//  Movement.cpp
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 7/30/22.
//

#include "Movement.hpp"

Movement::Movement()
: _defaultCoordinateVal(.0f),
  _ndcOffsetX(_defaultCoordinateVal),
  _ndcOffsetY(_defaultCoordinateVal)
{}

Movement::~Movement() {};
