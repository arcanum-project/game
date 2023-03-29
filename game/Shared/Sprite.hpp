//  Created by Dmitrii Belousov on 9/3/22.
//

#pragma once

#include "Model.hpp"
#include "TextureController.hpp"
#include "Movable.hpp"
#include "Common/Gameplay.hpp"
#include "SpriteInstanceData.h"

class Sprite : public Model, public Movable {
public:
  Sprite(MTL::Device * const pDevice);
  ~Sprite() = default;
  
  inline const SpriteInstanceData& getInstanceData() const { return instanceData; }
  
  inline void update(float_t deltaTime) override {
	// Increasing delta time speeds up animation to match the original game
	deltaTime = deltaTime * 2;
	static float_t timeAtCurrentSprite{0.f};
	static unsigned char currentDirectionIndex{8};
	static uint32_t currentTextureGroupStartIndex{instanceData.walkTextureStartIndex};
	
	unsigned char newDirectionIndex = getDirectionIndex(position());
	if (currentDirectionIndex != newDirectionIndex)
	{
	  currentDirectionIndex = newDirectionIndex;
	  currentTextureGroupStartIndex = instanceData.walkTextureStartIndex + currentDirectionIndex * instanceData.walkTexturePixelData.getFrameNum();
	  instanceData.currentTextureIndex = currentTextureGroupStartIndex;
	  timeAtCurrentSprite = deltaTime;
	}
	// For how long we draw the same frame is calculated based on target FPS (60.f)
	const float_t spriteLifetime = (instanceData.walkTexturePixelData.getKeyFrame() + 1) / 60.f;
	
	const bool bShowNextAnimationFrame = (timeAtCurrentSprite + deltaTime) > spriteLifetime ? true : false;
	instanceData.currentTextureIndex = bShowNextAnimationFrame ? instanceData.currentTextureIndex + 1 : instanceData.currentTextureIndex;
	timeAtCurrentSprite = bShowNextAnimationFrame ? timeAtCurrentSprite + deltaTime - spriteLifetime : timeAtCurrentSprite + deltaTime;
	if (instanceData.currentTextureIndex - currentTextureGroupStartIndex == instanceData.walkTexturePixelData.getFrameNum())
	  instanceData.currentTextureIndex = currentTextureGroupStartIndex;
	const Frame& newFrame = instanceData.walkTexturePixelData.frames().at(instanceData.currentTextureIndex - instanceData.walkTextureStartIndex);
	instanceData.currentFrameCenterX = newFrame.cx;
	instanceData.currentFrameCenterY = newFrame.cy;
	instanceData.currentTextureWidth = newFrame.imgWidth;
	instanceData.currentTextureHeight = newFrame.imgHeight;
	
	const glm::vec3 defaultPosition = Gameplay::getWorldTranslationFromTilePosition(GameplaySettings::CharacterStartRow, GameplaySettings::CharacterStartColumn) * glm::vec4(0.f, 0.f, 0.f, 1.f);
	if (position().x == 0.f && position().y == 0.f && position().z == 0.f)
	{
	  setPosition(defaultPosition);
	}
	else
	{
	  glm::vec3 outPositionWorld;
	  if (move(outPositionWorld, position(), deltaTime * GameplaySettings::CharacterWalkingSpeed))
	  {
		setPosition(outPositionWorld);
	  }
	}
  }
  
private:
  SpriteInstanceData instanceData;
  
  void populateVertexData() override;
  void loadTextures() override;
  void makeTexturesFromArt(const char * name, const char * type);
};
