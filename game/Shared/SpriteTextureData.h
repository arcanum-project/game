#pragma once

#include "PixelData.hpp"

struct SpriteTextureData {
  SpriteTextureData()
  : standTexturePixelData(new PixelData()),
	walkTexturePixelData(new PixelData())
  {
  };
  
  ~SpriteTextureData()
  {
	delete standTexturePixelData;
	delete walkTexturePixelData;
  };
  
  // Loaded assets data
  const char* artName;
  uint8_t frameIndex;
  uint8_t paletteIndex;
  uint16_t standTextureStartIndex;
  PixelData* standTexturePixelData;
  uint16_t walkTextureStartIndex;
  PixelData* walkTexturePixelData;
  uint8_t currentDirectionIndex;
};
