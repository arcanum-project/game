//
//  GameScene.cpp
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 7/24/22.
//

#include "glm/vec3.hpp"

#include "GameScene.hpp"
#include "Tile.hpp"
#include "IsometricCamera.hpp"
#include "Constants.h"

GameScene::GameScene(MTL::Device * const pDevice)
: _pDevice(pDevice),
  _pTile(std::make_shared<Tile>(pDevice, RenderingConstants::NumOfTilesPerSector, RenderingConstants::MaxBuffersInFlight)),
//  _pCharacter(std::make_shared<Character>(pDevice)),
  _pCharacter(),
  _models(std::vector<const std::shared_ptr<Model>> { _pTile }),
  _pCamera(std::make_unique<IsometricCamera>()) {
	_pCamera->setScale(0.1f);
	_pCamera->setPosition(glm::vec3(.0f, .0f, -50.0f));
	// Make isometric projection via rotations. Based on this: https://structuralcalc.com/is-there-math-in-drawings/
	_pCamera->setRotation(glm::vec3(glm::radians(35.26f), glm::radians(-135.0f), .0f));
}
