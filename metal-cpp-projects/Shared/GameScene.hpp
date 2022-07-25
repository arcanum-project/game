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

#include "Model.hpp"
#include "IsometricCamera.hpp"
#include "Metal/MTLDevice.hpp"

class GameScene {
public:
  GameScene(MTL::Device * const pDevice);
  ~GameScene();
  
  inline const std::vector<const Model * const> & models() { return _models; }
  inline Camera * const pCamera() { return _pCamera; }
  inline void update(const float_t & width, const float_t & height) { _pCamera->update(width, height); }
  inline void update(const float_t & deltaTime) { _pCamera->update(deltaTime); }

private:
  MTL::Device * const _pDevice;
  const Model * const _pTrain;
  const std::vector<const Model * const> _models;
  Camera * const _pCamera;
};

#endif /* GameScene_hpp */
