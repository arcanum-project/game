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

Sprite::Sprite()
: timeAtCurrentTexture(0.f),
  currentDirectionIndex(8)
{
}

void Sprite::update(float_t deltaTime)
{
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
