//
//  BMPImporter.hpp
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 7/31/22.
//

#ifndef BMPImporter_hpp
#define BMPImporter_hpp

#include <stdio.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

#include "Common/ResourceBundle.hpp"
#include "PixelData.hpp"

class BMPImporter {
public:
  static const PixelData import(const char * resourceName, const char * resourceType);
private:
  static const uint32_t readUpTo4Bytes(std::ifstream & file, const uint32_t start, const uint32_t length);
};

#endif /* BMPImporter_hpp */
