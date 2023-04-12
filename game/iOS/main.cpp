//
//  main.m
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 7/24/22.
//

#include <iostream>
#include <SDL.h>
#include <Metal/MTLDevice.hpp>
#include <QuartzCore/CAMetalLayer.hpp>

#include "Renderer.hpp"

int main(int argc, char * argv[]) {
  SDL_SetHint(SDL_HINT_RENDER_DRIVER, "metal");
  if (SDL_Init(SDL_INIT_VIDEO) < 0) SDL_Log("SDL could not be initialized: %s\n", SDL_GetError());
  else SDL_Log("SDL video system is ready to go!\n");
  
  int screen = 0;
  int modes = SDL_GetNumDisplayModes(screen);
  for (int i = 0; i < modes; i++) {
	  SDL_DisplayMode mode;
	  SDL_GetDisplayMode(screen, i, &mode);
	  printf("%dx%d\n", mode.w, mode.h);
  }
  
  // This will determine max window height and wight for the current device
  SDL_DisplayMode displayMode;
  SDL_GetCurrentDisplayMode(0, &displayMode);
  
  SDL_Window* window = SDL_CreateWindow("Arcanum", 0, 0, displayMode.w, displayMode.h, 0);
  
  SDL_Renderer* sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
  CA::MetalLayer* layer = (CA::MetalLayer*)SDL_RenderGetMetalLayer(sdlRenderer);
  
  int drawableWidth, drawableHeight;
  SDL_Metal_GetDrawableSize(window, &drawableWidth, &drawableHeight);
  
  MTL::TextureDescriptor* depthTextureDescriptor = MTL::TextureDescriptor::texture2DDescriptor(MTL::PixelFormatDepth32Float, drawableWidth, drawableHeight, false);
  depthTextureDescriptor->setStorageMode(MTL::StorageModePrivate);
  depthTextureDescriptor->setUsage(MTL::TextureUsageRenderTarget);
  MTL::Texture* depthTexture = layer->device()->newTexture(depthTextureDescriptor);
  depthTextureDescriptor->release();
  depthTexture->setLabel(NS::String::string("DepthTexture", NS::UTF8StringEncoding));
  
  Renderer* renderer = new Renderer(layer->device());
  // Necessary to initialize uniforms
  renderer->drawableSizeWillChange(drawableWidth, drawableHeight);
  
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
	
	NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();
	
	CA::MetalDrawable* nextDrawable = layer->nextDrawable();
	renderer->drawFrame(nextDrawable, depthTexture);
	
	pool->release();
  }

  depthTexture->release();
  delete renderer;
  SDL_DestroyRenderer(sdlRenderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  
  return 0;
}
