//  Created by Dmitrii Belousov on 9/3/22.
//

#pragma once

#include "Model.hpp"
#include "TextureController.hpp"
#include "Movable.hpp"
#include "Common/Gameplay.hpp"
#include "SpriteInstanceData.h"

class Sprite : public Transformable, public Movable {
public:
  Sprite();
  ~Sprite() = default;
  
  inline const SpriteInstanceData& getInstanceData() const { return instanceData; }
  
  void update(float_t deltaTime);
  
private:
  SpriteInstanceData instanceData;
  
  void loadTextures();
  void makeTexturesFromArt(const char * name, const char * type);
};
