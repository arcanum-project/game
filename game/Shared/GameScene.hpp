//
//  GameScene.hpp
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 7/24/22.
//
#pragma once

#include <stdio.h>
#include <vector>
#include <memory>
#include <Metal/MTLDevice.hpp>

#include "Tile.hpp"
#include "Sprite.hpp"
#include "IsometricCamera.hpp"

class GameScene {
public:
  GameScene(MTL::Device * const pDevice);
  ~GameScene();
  inline Tile* getTile() const { return tile; }
  inline const std::vector<Sprite*>& getSprites() const { return sprites; }
  inline const std::unique_ptr<Camera> & pCamera() { return _pCamera; }
  inline void update(const float_t & width, const float_t & height) { _pCamera->update(width, height); }
  inline void update(const float_t & deltaTime) {
	_pCamera->update(deltaTime);
  }

private:
  MTL::Device* const _pDevice;
  Tile* tile;
  std::vector<Sprite*> sprites;
  const std::unique_ptr<Camera> _pCamera;
};
