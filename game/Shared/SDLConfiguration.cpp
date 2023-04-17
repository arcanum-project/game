//

#include "SDLConfiguration.h"

SDLConfiguration::SDLConfiguration()
: window(nullptr),
  platformRenderer(nullptr)
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0) SDL_Log("SDL could not be initialized: %s\n", SDL_GetError());
  
  // This will determine max window height and wight for the current device
  SDL_DisplayMode displayMode;
  SDL_GetCurrentDisplayMode(0, &displayMode);
  
  window = SDL_CreateWindow("Arcanum", 0, 0, displayMode.w, displayMode.h, 0);
  
  platformRenderer = new PlatformRenderer(window);
  
  bool gameIsRunning = true;
  while (gameIsRunning)
  {
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
	  switch (event.type)
	  {
		case SDL_QUIT:
		  gameIsRunning= false;
		  break;
		default:
		  break;
	  }
	}
	
	platformRenderer->drawFrame();
  }
}

SDLConfiguration::~SDLConfiguration()
{
  delete platformRenderer;
  
  SDL_DestroyWindow(window);
  SDL_Quit();
}
