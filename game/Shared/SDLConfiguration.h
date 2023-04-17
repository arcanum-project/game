//

#pragma once

#include <SDL.h>

#include "PlatformRenderer.h"

class SDLConfiguration
{
public:
  SDLConfiguration();
  ~SDLConfiguration();

private:
  SDL_Window* window;
  PlatformRenderer* platformRenderer;
};
