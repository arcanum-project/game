//
//  BMPImporter.cpp
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 7/31/22.
//

#include "BMPImporter.hpp"

const PixelData BMPImporter::import(const char * resourceName, const char * resourceType) {
  std::ifstream file;
  PixelData pixelData = PixelData();
  try {
	file.open(ResourceBundle::absolutePath(resourceName, resourceType), std::ifstream::in);
	if (file.is_open()) {
	  // BMP is parsed according to explanation from here: https://www.digicamsoft.com/bmp/bmp.html
	  // Read image width
	  const uint8_t pImgWidth = 0x12;
	  const uint32_t imgWidth = readUpTo4Bytes(file, pImgWidth, 4);
	  pixelData.imgWidth = imgWidth;
	  
	  // Read image height
	  const uint8_t pImgHeight = 0x16;
	  const uint32_t imgHeight = readUpTo4Bytes(file, pImgHeight, 4);
	  pixelData.imgHeight = imgHeight;
	  
	  // Resize the container to hold pixel data.
	  // Size = num of pixels * size of pixel = img width * img height * 4 bytes.
	  const uint32_t pixelCount = imgWidth * imgHeight;
	  pixelData.pixels.reserve(pixelCount * 4);
	  
	  // Read color table
	  // Start of BITMAPINFOHEADER
	  const uint8_t pBitmapInfo = 0x0E;
	  const uint32_t offsetFromBitmapInfoToColorTable = readUpTo4Bytes(file, pBitmapInfo, 4);
	  
	  // Read how many bits per pixel this BMP has
	  // Start of a WORD with number of bits per pixel
	  const uint8_t pBitsPerPixel = 0x1C;
	  const uint32_t bitsPerPixel = readUpTo4Bytes(file, pBitsPerPixel, 2);
	  
	  // Move to color table. Its location: start of BITMAPINFOHEADER + size of BITMAPINFOHEADER
	  file.seekg(pBitmapInfo + offsetFromBitmapInfoToColorTable, std::ios::beg);
	  // Color table has palettes. Number of palettes = 2 ^ number of bits per pixel.
	  // Each palette is 4 bytes long: one byte for Blue, one for Green, one for Red and one is reserved.
	  // Therefore size of color table = number of palettes * size of palette
	  uint32_t colorTableSize = (std::pow(2.0f, (float_t) bitsPerPixel)) * 4;
	  std::vector<uint8_t> colorTable = std::vector<uint8_t>(colorTableSize);
	  file.read(reinterpret_cast<char*>(colorTable.data()), colorTableSize);
	  /*
	   We intend to use this BMP as a Metal texture - and Metal requires all textures to be formatted with a specific MTLPixelFormat value. The pixel format describes the layout of pixel data in the texture. We will use the MTLPixelFormatBGRA8Unorm pixel format, which uses 32 bits per pixel, arranged into 8 bits per component, in blue, green, red, and alpha order.
	   From here: https://developer.apple.com/documentation/metal/textures/creating_and_sampling_textures?language=objc
	   */
	  // To comply with MTLPixelFormatBGRA8Unorm, we will set fourth byte in each palette to 255 to indicate fully opaque pixel.
	  for (size_t i = 1; i <= colorTable.size(); i++) {
		if (i % 4 == 0) {
		  colorTable.at(i - 1) = 0xFF;
		}
	  }
	  
	  // Read pixels
	  /*
	   The bits representing the bitmap pixels are packed in rows(also known as strides or scan lines). The size of each row is rounded up to a multiple of 4 bytes (a 32-bit DWORD) by padding.
	   This means we need to read pixels row by row, offsetting each row by padding if any.
	   Padding = row length % 4
	   Another thing - rows are actually stored in reverse order - i.e. we will read rows from bottom to top
	   */
	  const uint32_t padding = imgWidth % 4;
	  uint64_t rowsRead {1};
	  while (rowsRead <= imgHeight) {
		uint64_t rowPixelsRead {1};
		while (rowPixelsRead <= imgWidth) {
		  uint8_t pixelContainer[1]{};
		  file.read(reinterpret_cast<char*>(pixelContainer), 1);
		  uint16_t pixel {};
		  memcpy(& pixel, pixelContainer, 1);
		  // Each pixel is stored as an index into color table that specifies a palette for that pixel.
		  for (size_t i = pixel * 4; i < pixel * 4 + 4; ++i) {
		   pixelData.pixels.push_back(colorTable.at(i));
		  }
		  ++rowPixelsRead;
		}
		// Move file pointer forward by row offset
		file.seekg(padding, std::ios::cur);
		++rowsRead;
	  }
	  
	  file.close();
	}
  } catch (std::system_error & e) {
	if (file.is_open())
	  file.close();
	std::cerr << e.code().message() << std::endl;
	throw;
  }
  
  return pixelData;
}

/**
 Read up to 4 bytes starting from a specified position in a file.
 
 Note: this method moves the file pointer by a number of bytes read
 
 @param file - file to read from
 @param start - position in a file to read from inclusively
 @param length - number of bytes to read.
 @returns 4-byte unsigned integer
 */
const uint32_t BMPImporter::readUpTo4Bytes(std::ifstream & file, const uint32_t start, const uint32_t length) {
  file.seekg(start, std::ios::beg);
  std::vector<uint8_t> container = std::vector<uint8_t>(length);
  file.read(reinterpret_cast<char*>(container.data()), length);
  uint32_t result {};
  memcpy(& result, container.data(), length);
  return result;
}
