//  Created by Dmitrii Belousov on 9/3/22.
//

#pragma once

#include <Metal/MTLDevice.hpp>

#include "Transformable.hpp"
#include "Movable.hpp"
#include "SpriteInstanceData.h"

class Sprite : public Transformable, public Movable {
public:
  Sprite(MTL::Device* device);
  ~Sprite() = default;
  
  inline const SpriteInstanceData& getInstanceData() const { return instanceData; }
  
  void update(float_t deltaTime);
  
private:
  SpriteInstanceData instanceData;
  MTL::Device* device;
  
  void loadTextures();
  void makeTexturesFromArt(const char * name, const char * type);
};
