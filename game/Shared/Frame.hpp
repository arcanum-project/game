//
//  Frame.h
//  game
//
//  Created by Dmitrii Belousov on 10/13/22.
//

#ifndef Frame_h
#define Frame_h

#include <stdio.h>
#include <vector>

// Public represenation of internal ArtFrameHeader
struct Frame {
  uint32_t imgWidth;
  uint32_t imgHeight;
  std::vector<uint8_t> pixels;
  uint32_t cx;
  uint32_t cy;
  uint32_t dx;
  uint32_t dy;
};

#endif /* Frame_h */
