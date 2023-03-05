//

#include <glm/mat4x4.hpp>

#include "Gameplay.hpp"
#include "Math.hpp"
#include "GameSettings.hpp"

glm::mat4x4 Gameplay::getWorldTranslationFromTilePosition(const uint16_t row, const uint16_t column)
{
  return Math::getInstance().translation(row * RenderingSettings::TileLength, .0f, column * RenderingSettings::TileLength);
}
