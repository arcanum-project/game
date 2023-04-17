//

#pragma once

#include <SDL.h>

#include "PlatformRenderer.h"

class SDLConfiguration
{
public:
  //  constructors
  static void initialize();
  SDLConfiguration(SDLConfiguration const&) = delete;
  void operator=(SDLConfiguration const&) = delete;
  
private:
  SDLConfiguration();
  ~SDLConfiguration();
  
  SDL_Window* window;
  PlatformRenderer* platformRenderer;
};
