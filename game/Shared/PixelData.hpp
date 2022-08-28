//
//  PixelData.h
//  metal-cpp-projects
//
//  Created by Dmitrii Belousov on 8/3/22.
//

#ifndef PixelData_h
#define PixelData_h

#include <stdio.h>
#include <vector>

struct PixelData {
  PixelData()
  : imgWidth(),
	imgHeight(),
	pixels(std::vector<uint8_t>()) {};
  
  uint32_t imgWidth;
  uint32_t imgHeight;
  std::vector<uint8_t> pixels;
};

#endif /* PixelData_h */
