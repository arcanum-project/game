//

#include "GameSettings.h"

namespace GameplaySettings
{
  float CharacterWalkingSpeed = 3.f;
  float CameraMovementSpeed = 6.f;
  unsigned char CharacterStartRow = 30;
  unsigned char CharacterStartColumn = 32;
};

namespace RenderingSettings
{
  const unsigned char MaxBuffersInFlight = 3;
  const unsigned short NumOfTilesPerSector = 4096;
  const unsigned char NumOfTilesPerRow = 64;
  const float TileLength = 2.f;
  const float DirectionEpsilonNDC = 0.1f;
  const float WorldScalar = 9.f;
};
