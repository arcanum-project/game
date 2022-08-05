//
//  GameScene.cpp
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 7/24/22.
//

#include "glm/vec3.hpp"

#include "GameScene.hpp"
#include "ObjModelImporter.hpp"
#include "IsometricCamera.hpp"
#include "Constants.hpp"

GameScene::GameScene(MTL::Device * const pDevice)
: _pDevice(pDevice),
  _pTile(importTile(_pDevice)),
  _models(std::vector<const std::shared_ptr<const Model>> { _pTile }),
  _pCamera(std::make_unique<IsometricCamera>()) {
	_pCamera->setScale(0.25f);
	_pCamera->setPosition(glm::vec3(22.25f, 12.75f, -50.0f));
	// Make isometric projection via rotations. Based on this: https://structuralcalc.com/is-there-math-in-drawings/
	_pCamera->setRotation(glm::vec3(glm::radians(35.26f), glm::radians(-45.0f), .0f));
}

const std::shared_ptr<const Model> GameScene::importTile(MTL::Device * const pDevice) {
  const std::unique_ptr<const ImportedModelData> imported = ObjModelImporter().import("tile-adjusted-uv-2", "obj");
  return std::make_shared<const Model>(imported->vertexData, imported->indices, pDevice, RenderingConstants::NumOfTilesPerSector, RenderingConstants::MaxBuffersInFlight, "tiletexture", "bmp");
}
