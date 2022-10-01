//
//  ArtImporter.cpp
//  game
//
//  Created by Dmitrii Belousov on 9/30/22.
//

#include <fstream>
#include <iostream>

#include "ArtImporter.hpp"
#include "Common/ResourceBundle.hpp"

// Based on: https://github.com/AxelStrem/ArtConverter
const PixelData ArtImporter::importFrame(const char * artName, uint32_t frameNum) {
  std::ifstream file;
  try {
	file.open(ResourceBundle::absolutePath(artName, "art"));
	ArtFile af;
	file.read(reinterpret_cast<char*>(&af.header), sizeof(af.header));
	// Get total number of palettes from header
	af.palettes = 0;
	for (const Color existingPalette : af.header.existingPalettes) {
	  if (af.isInPalette(existingPalette)) af.palettes++;
	}
	for (ushort i = 0; i < af.palettes; ++i) {
	  af.paletteData.push_back(ColorTable());
	  file.read(reinterpret_cast<char*>(&af.paletteData.back()), sizeof(ColorTable));
	}
	af.isAnimated = ((af.header.h0[0] & 0x1) == 0);
	af.frames = af.isAnimated ? af.header.frameNum * 8 : af.header.frameNum;
	for (ushort i = 0; i < af.frames; ++i) {
	  af.frameData.push_back(ArtFrame());
	  af.frameData.back().loadHeader(file);
	}
	for (ArtFrame& frame : af.frameData) {
	  frame.load(file);
	  frame.decode();
	}
	
	file.close();
  } catch (std::system_error & e) {
	if (file.is_open())
	  file.close();
	std::cerr << e.code().message() << std::endl;
	throw;
  }
  
  throw std::runtime_error("TODO I should return actual PixelData here!");
}

void ArtImporter::ArtFrame::decode() {
  pixels = std::vector<std::vector<uint8_t>>(header.height, std::vector<uint8_t>(header.width));
  if (header.size < (header.height * header.width)) {
	for (uint32_t p = 0; p < header.size; ++p) {
	  const uint8_t ch = data[p];
	  if (ch & 0x80) {
		ushort toCopy = ch & 0x7F;
		while (toCopy--) {
		  p++;
		  pixels[py][px] = data[p];
		  inc();
		}
	  } else {
		ushort toClone = ch & 0x7F;
		p++;
		while (toClone--) {
		  pixels[py][px] = data[p];
		  inc();
		}
	  }
	}
  } else {
	for (uint32_t p = 0; p < header.size; ++p) {
	  pixels[py][px] = data[p];
	  inc();
	}
  }
}
