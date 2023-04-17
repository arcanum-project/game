//

#pragma once

#include <SDL.h>

class GLRenderer
{
public:
  GLRenderer(SDL_Window* window);
  ~GLRenderer();
  
  void drawFrame();
  
private:
  SDL_Window* window;
  SDL_GLContext glContext;
};
