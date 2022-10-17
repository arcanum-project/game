//
//  Character.cpp
//  iOS
//
//  Created by Dmitrii Belousov on 9/3/22.
//

#include "Character.hpp"
#include "ObjModelImporter.hpp"
#include "TextureController.hpp"

Character::Character(MTL::Device * const pDevice)
: Model(pDevice, 1),
  _textureIndex() {
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

void Character::loadTextures() {
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
