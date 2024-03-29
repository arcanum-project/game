//

#pragma once

namespace GameplaySettings
{
  extern float CharacterWalkingSpeed;
  extern float CameraMovementSpeed;
  extern unsigned char CharacterStartRow;
  extern unsigned char CharacterStartColumn;
};

namespace RenderingSettings
{
  extern const unsigned char MaxBuffersInFlight;
  extern const unsigned short NumOfTilesPerSector;
  extern const unsigned char NumOfTilesPerRow;
  extern const float TileLength;
  extern const float DirectionEpsilonNDC;
  extern const float WorldScalar;
};
