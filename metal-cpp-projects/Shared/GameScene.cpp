//
//  GameScene.cpp
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 7/24/22.
//

#include "GameScene.hpp"
#include "ObjModelImporter.hpp"
#include "IsometricCamera.hpp"
#include "glm/vec3.hpp"
#include "glm/gtc/constants.hpp"

GameScene::GameScene(MTL::Device * const pDevice)
: _pDevice(pDevice),
  _pTrain(ObjModelImporter().import(_pDevice, "train", "obj")),
  _models(std::vector<const Model * const> { _pTrain }),
  _pCamera(new IsometricCamera()) {
	_pCamera->setScale(0.5f);
	_pCamera->setPosition(glm::vec3(0, -0.5, 2));
	// Make isometric projection via rotations. Based on this: https://structuralcalc.com/is-there-math-in-drawings/
	_pCamera->setRotation(glm::vec3(glm::radians(35.26f), glm::radians(45.0f), .0f));
}

GameScene::~GameScene() {
  delete _pCamera;
  delete _pTrain;
}
