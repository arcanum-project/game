//
//  ArtImporter.hpp
//  game
//
//  Created by Dmitrii Belousov on 9/30/22.
//

#ifndef ArtImporter_hpp
#define ArtImporter_hpp

#include <stdio.h>
#include <vector>
#include <utility>
#include <fstream>

#include "Frame.hpp"
#include "PixelData.hpp"

class ArtImporter {
  // Private members
  struct Color {
	uint8_t b;
	uint8_t g;
	uint8_t r;
	uint8_t a;
  };
  
  struct ArtHeader {
	uint32_t h0[3];
	Color existingPalettes[4];
	uint32_t keyFrame;
	uint32_t frameNum;
	Color paletteData1[8];
	Color paletteData2[8];
	Color paletteData3[8];
  };
  
  struct ColorTable {
	Color colors[256];
  };
  
  struct ArtFrameHeader {
	uint32_t width;
	uint32_t height;
	uint32_t size;
	int cx;
	int cy;
	int dx;
	int dy;
  };
  
  class ArtFrame {
  public:
	ArtFrame();
	~ArtFrame();
	ArtFrameHeader header;
	std::vector<uint8_t> data;
	std::vector<uint8_t> pixels;
	uint32_t px;
	uint32_t py;
	inline void loadHeader(std::ifstream& file) {
	  file.read(reinterpret_cast<char*>(&header), sizeof(header));
	}
	inline void load(std::ifstream& file) {
	  data = std::vector<uint8_t>(header.size);
	  file.read(reinterpret_cast<char*>(data.data()), header.size);
	}
	
	void decode();
	
	inline void inc() {
	  px++;
	  if (px == header.width) {
		px = 0;
		py++;
	  }
	}
  };
  
  class ArtFile {
  public:
	// Fields
	ArtHeader header;
	bool isAnimated;
	uint16_t palettes;
	std::vector<ColorTable> paletteData;
	uint16_t frames;
	std::vector<ArtFrame> frameData;
	// Methods
	inline const bool isInPalette(const Color& col) const { return (col.a | col.b | col.g | col.r) != 0; }
  };
public:
  static const PixelData importArt(const char * artName, const char * artType);
};

#endif /* ArtImporter_hpp */
