//
//  TextureController.cpp
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 8/3/22.
//

#include "TextureController.hpp"

const TextureController & TextureController::instance(MTL::Device * const pDevice) {
  static TextureController instance(pDevice);
  return instance;
}

TextureController::TextureController(MTL::Device * const pDevice)
: _pDevice(pDevice) {}

TextureController::~TextureController() {}
