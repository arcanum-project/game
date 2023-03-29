//
//  GameScene.cpp
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 7/24/22.
//

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <utility>

#include "GameScene.hpp"
#include "Tile.hpp"
#include "IsometricCamera.hpp"
#include "Common/Gameplay.hpp"
#include "GameSettings.h"

GameScene::GameScene(MTL::Device * const pDevice)
: _pDevice(pDevice),
  tile(new Tile(pDevice, RenderingSettings::NumOfTilesPerSector, RenderingSettings::MaxBuffersInFlight)),
  sprites(std::vector<Sprite*>()),
  _pCamera(std::make_unique<IsometricCamera>())
{
  // Dynamic memory allocation is bad, because slow. To avoud that, always allocate just enough memory.
  sprites.reserve(1);
  sprites.push_back(new Sprite(_pDevice));
  
  _pCamera->setScale(RenderingSettings::WorldScalar);
  const glm::mat4x4 cameraPos = Gameplay::getWorldTranslationFromTilePosition(GameplaySettings::CharacterStartRow, GameplaySettings::CharacterStartColumn);
  _pCamera->setPosition(std::move(glm::vec3(cameraPos[3].x, cameraPos[3].y, cameraPos[3].z)));
  // Make isometric projection via rotations. Based on this: https://structuralcalc.com/is-there-math-in-drawings/
  _pCamera->setRotation(glm::vec3(glm::radians(35.26f), glm::radians(-135.0f), .0f));
}

GameScene::~GameScene()
{
  delete tile;
  for (Sprite* sprite : sprites)
	delete sprite;
}
