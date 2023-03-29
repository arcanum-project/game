//
//  Character.cpp
//  iOS
//
//  Created by Dmitrii Belousov on 9/3/22.
//

#include <vector>

#include "Sprite.hpp"
#include "ObjModelImporter.hpp"
#include "ArtImporter.hpp"
#include "TextureController.hpp"
#include "Gameplay.hpp"

Sprite::Sprite(MTL::Device* device)
: instanceData(),
  device(device)
{
  loadTextures();
}

void Sprite::makeTexturesFromArt(const char * name, const char * type) {
  PixelData pd = ArtImporter::importArt(name, "art");
//  const uint8_t defaultFrameIndex = 3;
  const uint8_t defaultPaletteIndex = 2;
  bool isTextureIndexSet {false};
  for (ushort i = 0; i < pd.frames().size(); ++i) {
	const std::vector<uint8_t> bgras = pd.bgraFrameFromPalette(i, defaultPaletteIndex);
	// Can pass nullptr because at this moment TextureController has already been initialized
	// Using nullptr as a hack to avoid keeping a reference to MTL::Device
	TextureController& txController = TextureController::instance(device);
	const uint16_t txIndex = txController.loadTexture(name, pd.frames().at(i).imgHeight, pd.frames().at(i).imgWidth, bgras.data());
	instanceData.artName = name;
	instanceData.frameIndex = i;
	instanceData.paletteIndex = defaultPaletteIndex;
	// We do this to ensure that we have an index of the first frame.
	// Since frames are stored contiguously, with start frame and offset we can get any frame we need.
	if (!isTextureIndexSet) {
	  // Get pointer to last char in name - it will define what type of animation this texture is for
	  while (*name++ != '\0')
		;
	  // Have to do it, because after while loop name points at the next char after '\0'
	  name -= 2;
	  switch (*name) {
		case 'a':
		  instanceData.standTextureStartIndex = txIndex;
		  instanceData.standTexturePixelData = pd;
		  isTextureIndexSet = true;
		  break;
		case 'b':
		  instanceData.walkTextureStartIndex = txIndex;
		  instanceData.walkTexturePixelData = pd;
		  isTextureIndexSet = true;
		  break;
		default:
		  break;
	  }
	}
  }
  instanceData.currentTextureIndex = instanceData.walkTextureStartIndex;
}

void Sprite::loadTextures() {
  makeTexturesFromArt("hmfc2xab", "art");
}

void Sprite::update(float_t deltaTime)
{
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
