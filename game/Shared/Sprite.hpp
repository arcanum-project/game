//  Created by Dmitrii Belousov on 9/3/22.
//

#pragma once

#include "Transformable.hpp"
#include "Movable.hpp"
#include "SpriteTextureData.h"

class Sprite : public Transformable, public Movable {
public:
  Sprite();
  ~Sprite() = default;
  
  /**
	For how many seconds the current sprite texture is being rendered.
   */
  inline float_t getTimeAtCurrentTexture() const { return timeAtCurrentTexture; }
  inline void setTimeAtCurrentTexture(float_t newValue)  { timeAtCurrentTexture = newValue; }
  /**
	To which of the eight possible directions the current sprite texture belongs.
   */
  inline uint8_t getCurrentDirectionIndex() const { return currentDirectionIndex; }
  inline void setCurrentDirectionIndex(float_t newValue)  { currentDirectionIndex = newValue; }
  
  void update(float_t deltaTime);
  
private:
  float_t timeAtCurrentTexture;
  uint8_t currentDirectionIndex;
};
