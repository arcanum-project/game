#pragma once

#include "PixelData.hpp"

struct SpriteInstanceData {
  // Loaded assets data
  const char * artName;
  uint8_t frameIndex;
  uint8_t paletteIndex;
  uint16_t standTextureStartIndex;
  PixelData standTexturePixelData;
  uint16_t walkTextureStartIndex;
  PixelData walkTexturePixelData;
  
  // Current texture metadata
  uint32_t currentTextureIndex;
  int32_t currentFrameCenterX;
  int32_t currentFrameCenterY;
  uint32_t currentTextureWidth;
  uint32_t currentTextureHeight;
};
