//
//  Character.cpp
//  iOS
//
//  Created by Dmitrii Belousov on 9/3/22.
//

#include <vector>

#include "Sprite.hpp"
#include "ObjModelImporter.hpp"
#include "ArtImporter.hpp"

Sprite::Sprite(MTL::Device * const pDevice)
: Model(pDevice, 1),
  instanceData()
{
  loadTextures();
}

void Sprite::populateVertexData()
{
}

void Sprite::makeTexturesFromArt(const char * name, const char * type) {
  PixelData pd = ArtImporter::importArt(name, "art");
//  const uint8_t defaultFrameIndex = 3;
  const uint8_t defaultPaletteIndex = 2;
  bool isTextureIndexSet {false};
  for (ushort i = 0; i < pd.frames().size(); ++i) {
	const std::vector<uint8_t> bgras = pd.bgraFrameFromPalette(i, defaultPaletteIndex);
	const uint16_t txIndex = TextureController::instance(pDevice()).loadTexture(name, pd.frames().at(i).imgHeight, pd.frames().at(i).imgWidth, bgras.data());
	instanceData.artName = name;
	instanceData.frameIndex = i;
	instanceData.paletteIndex = defaultPaletteIndex;
	// We do this to ensure that we have an index of the first frame.
	// Since frames are stored contiguously, with start frame and offset we can get any frame we need.
	if (!isTextureIndexSet) {
	  // Get pointer to last char in name - it will define what type of animation this texture is for
	  while (*name++ != '\0')
		;
	  // Have to do it, because after while loop name points at the next char after '\0'
	  name -= 2;
	  switch (*name) {
		case 'a':
		  instanceData.standTextureStartIndex = txIndex;
		  instanceData.standTexturePixelData = pd;
		  isTextureIndexSet = true;
		  break;
		case 'b':
		  instanceData.walkTextureStartIndex = txIndex;
		  instanceData.walkTexturePixelData = pd;
		  isTextureIndexSet = true;
		  break;
		default:
		  break;
	  }
	}
  }
  instanceData.currentTextureIndex = instanceData.walkTextureStartIndex;
}

void Sprite::loadTextures() {
  makeTexturesFromArt("hmfc2xab", "art");
}
