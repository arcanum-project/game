//
//  GameScene.hpp
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 7/24/22.
//

#ifndef GameScene_hpp
#define GameScene_hpp

#include <stdio.h>
#include <vector>
#include <memory>
#include <Metal/MTLDevice.hpp>

#include "Model.hpp"
#include "Tile.hpp"
#include "Sprite.hpp"
#include "IsometricCamera.hpp"

class GameScene {
public:
  GameScene(MTL::Device * const pDevice);
  ~GameScene();
  inline const std::vector<Sprite*>& getSprites() const { return sprites; }
  inline const std::vector<const std::shared_ptr<Model>> & models() { return _models; }
  inline const std::unique_ptr<Camera> & pCamera() { return _pCamera; }
  inline void update(const float_t & width, const float_t & height) { _pCamera->update(width, height); }
  inline void update(const float_t & deltaTime) {
	_pCamera->update(deltaTime);
  }

private:
  MTL::Device * const _pDevice;
  const std::shared_ptr<Tile> _pTile;
  std::vector<Sprite*> sprites;
  const std::vector<const std::shared_ptr<Model>> _models;
  const std::unique_ptr<Camera> _pCamera;
  
  const std::shared_ptr<const Model> importTile(MTL::Device * const pDevice);
  const std::shared_ptr<Sprite> importCharacter(MTL::Device * const pDevice);
};

#endif /* GameScene_hpp */
