//
//  PixelData.cpp
//  game
//
//  Created by Dmitrii Belousov on 10/13/22.
//

#include "PixelData.hpp"

PixelData::PixelData()
: _palettes(std::vector<std::vector<uint8_t>>()),
  _frames(std::vector<Frame>()),
  _keyFrame()
{};

PixelData::~PixelData(){};

/**
 Return array of BGRA-formatted colors for a specific frame of a specific palette.
 */
const std::vector<uint8_t> PixelData::bgraFrameFromPalette(const uint16_t& frameNum, const uint8_t& paletteIndex) const {
  std::vector<uint8_t> bgras = std::vector<uint8_t>();
  bgras.reserve(1024);
  // Each pixel is an index into color table.
  // We now need to iterate over each such index and convert into actual BGRA color
  for (const uint8_t& pixelIndex : _frames.at(frameNum).pixels) {
	const std::vector<uint8_t>& palette = _palettes.at(paletteIndex);
	const uint8_t b = palette.at(pixelIndex * 4);
	const uint8_t g = palette.at(pixelIndex * 4 + 1);
	const uint8_t r = palette.at(pixelIndex * 4 + 2);
	const uint8_t a = palette.at(pixelIndex * 4 + 3);
	bgras.push_back(b);
	bgras.push_back(g);
	bgras.push_back(r);
	bgras.push_back(a);
  }
  return bgras;
};
