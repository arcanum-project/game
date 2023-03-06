//
//  PixelData.hpp
//  game
//
//  Created by Dmitrii Belousov on 10/13/22.
//

#ifndef PixelData_hpp
#define PixelData_hpp

#include <stdio.h>
#include <vector>

#include "Frame.hpp"

class PixelData {
public:
  inline std::vector<std::vector<uint8_t>>& palettes() { return _palettes; }
  inline std::vector<Frame>& frames() { return _frames; }
  inline uint32_t getKeyFrame() { return _keyFrame; }
  inline void setKeyFrame(const uint32_t keyFrame) { _keyFrame = keyFrame; }
  
  PixelData();
  ~PixelData();
  
  /**
   Return array of BGRA-formatted colors for a specific frame of a specific palette.
   */
  const std::vector<uint8_t> bgraFrameFromPalette(const uint16_t& frameNum, const uint8_t& paletteIndex) const;
  
private:
  std::vector<std::vector<uint8_t>> _palettes;
  std::vector<Frame> _frames;
  // Number of frames in a full animation cycle for a single movement direction. It is zero based, i.e. first frame = 0
  uint32_t _keyFrame;
};

#endif /* PixelData_hpp */
