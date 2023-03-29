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
const PixelData ArtImporter::importArt(const char * artName, const char * artType) {
  PixelData pd = PixelData();
  std::ifstream file;
  try {
	file.open(ResourceBundle::absolutePath(artName, "art"));
	ArtFile af;
	file.read(reinterpret_cast<char*>(&af.header), sizeof(af.header));
	pd.setKeyFrame(af.header.keyFrame);
	pd.setFrameNum(af.header.frameNum);
	// Get total number of palettes from header
	af.palettes = 0;
	for (const Color& existingPalette : af.header.existingPalettes) {
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
	
	// Make pixel data - public representation of imported art file
	// Add palettes
	for (const ColorTable& ct : af.paletteData) {
	  std::vector<uint8_t> p = std::vector<uint8_t>();
	  p.reserve(1024);
	  for (const Color& c : ct.colors) {
		/*
		 We intend to use this BMP as a Metal texture - and Metal requires all textures to be formatted with a specific MTLPixelFormat value. The pixel format describes the layout of pixel data in the texture. We will use the MTLPixelFormatBGRA8Unorm pixel format, which uses 32 bits per pixel, arranged into 8 bits per component, in blue, green, red, and alpha order.
		 From here: https://developer.apple.com/documentation/metal/textures/creating_and_sampling_textures?language=objc
		 */
		p.push_back(c.b);
		p.push_back(c.g);
		p.push_back(c.r);
		// To comply with MTLPixelFormatBGRA8Unorm, we will set fourth byte in each palette to 255 to indicate fully opaque pixel.
		p.push_back(0xFF);
	  }
	  pd.palettes().push_back(p);
	}
	// Add frames
	for (const ArtFrame& artf : af.frameData) {
	  Frame f;
	  f.imgWidth = artf.header.width;
	  f.imgHeight = artf.header.height;
	  f.pixels = std::move(artf.pixels);
	  f.cx = artf.header.cx;
	  f.cy = artf.header.cy;
	  f.dx = artf.header.dx;
	  f.dy = artf.header.dy;
	  pd.frames().push_back(f);
	}
  } catch (std::system_error& e) {
	if (file.is_open())
	  file.close();
	std::cerr << e.code().message() << std::endl;
	throw;
  }
  return pd;
}

ArtImporter::ArtFrame::ArtFrame()
: header(ArtFrameHeader {
	.width = 0,
	.height = 0,
	.size = 0,
	.cx = 0,
	.cy = 0,
	.dx = 0,
	.dy = 0
  }),
  data(std::vector<uint8_t>()),
  pixels(std::vector<uint8_t>()),
  px(0),
  py(0)
{};

ArtImporter::ArtFrame::~ArtFrame(){};

void ArtImporter::ArtFrame::decode() {
  pixels.reserve(header.height * header.width);
  if (header.size < header.height * header.width) {
	for (uint32_t p = 0; p < header.size; ++p) {
	  const uint8_t ch = data[p];
	  if (ch & 0x80) {
		ushort toCopy = ch & 0x7F;
		while (toCopy--) {
		  p++;
		  pixels.push_back(data[p]);
		}
	  } else {
		ushort toClone = ch & 0x7F;
		p++;
		while (toClone--) {
		  pixels.push_back(data[p]);
		}
	  }
	}
  } else {
	for (uint32_t p = 0; p < header.size; ++p) {
	  pixels.push_back(data[p]);
	}
  }
}
