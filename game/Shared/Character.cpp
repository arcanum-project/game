//
//  Character.cpp
//  iOS
//
//  Created by Dmitrii Belousov on 9/3/22.
//

#include <vector>

#include "Character.hpp"
#include "ObjModelImporter.hpp"
#include "ArtImporter.hpp"

Character::Character(MTL::Device * const pDevice)
: Model(pDevice, 1),
  _instanceData()
{
  populateVertexData();
  loadTextures();
}

Character::~Character() {
  pIndexBuffer()->release();
  pVertexBuffer()->release();
}

void Character::populateVertexData() {
  const std::unique_ptr<const ImportedModelData> tile = ObjModelImporter().import("character-5x5", "obj");
  setVertexData(tile->vertexData);
  setIndices(tile->indices);
  
  setVertexBuffer(pDevice()->newBuffer(vertexData().data(), vertexData().size() * sizeof(VertexData), MTL::ResourceStorageModeShared));
  setIndexBuffer(pDevice()->newBuffer(indices().data(), indices().size() * sizeof(uint16_t), MTL::ResourceStorageModeShared));
}

void Character::makeTexturesFromArt(const char * name, const char * type) {
  PixelData pd = ArtImporter::importArt(name, "art");
//  const uint8_t defaultFrameIndex = 3;
  const uint8_t defaultPaletteIndex = 2;
  bool isTextureIndexSet {false};
  for (ushort i = 0; i < pd.frames().size(); ++i) {
	const std::vector<uint8_t> bgras = pd.bgraFrameFromPalette(i, defaultPaletteIndex);
	const uint16_t txIndex = TextureController::instance(pDevice()).loadTexture(name, pd.frames().at(i).imgHeight, pd.frames().at(i).imgWidth, bgras.data());
	_instanceData.artName = name;
	_instanceData.frameIndex = i;
	_instanceData.paletteIndex = defaultPaletteIndex;
	// We do this to ensure that we have an index of the first frame.
	// Since frames are stored contiguously, with start frame and offset we can get any frame we need.
	if (!isTextureIndexSet) {
	  _instanceData.baseTextureIndex = txIndex;
	  _instanceData.currentTextureIndex = _instanceData.baseTextureIndex;
	  isTextureIndexSet = true;
	}
	_instanceData.pixelData = pd;
  }
}

void Character::loadTextures() {
//  const char* name = "ghmstxaa";
  const char* name = "hmfc2xaa";
  makeTexturesFromArt(name, "art");
//  _textureIndex = TextureController::instance(pDevice()).loadTexture("efmv1xaa_04", "bmp");
//  _textureIndex = TextureController::instance(pDevice()).loadTexture("efmv1xaa_03", "bmp");
//  _textureIndex = TextureController::instance(pDevice()).loadTexture("efmv1xaa_05", "bmp");
//  _textureIndex = TextureController::instance(pDevice()).loadTexture("hmfc2xaa_05", "bmp");
//  _textureIndex = TextureController::instance(pDevice()).loadTexture("efmv1xna_25", "bmp");
//  _textureIndex = TextureController::instance(pDevice()).loadTexture("efmv1xna_31", "bmp");
//  _textureIndex = TextureController::instance(pDevice()).loadTexture("ghmstxaa_0", "bmp");
//  _textureIndex = TextureController::instance(pDevice()).loadTexture("hmfc2xai_117", "bmp");
//  _textureIndex = TextureController::instance(pDevice()).loadTexture("hmfc2xai_06", "bmp");
//    _textureIndex = TextureController::instance(pDevice()).loadTexture("hgmv1xaa_06", "bmp");
//  _textureIndex = TextureController::instance(pDevice()).loadTexture("efmbnxak_80", "bmp");
//  _textureIndex = TextureController::instance(pDevice()).loadTexture("efmbnxak_43", "bmp");
}
