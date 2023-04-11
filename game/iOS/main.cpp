//
//  main.m
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 7/24/22.
//

#include <SDL.h>

int main(int argc, char * argv[]) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) SDL_Log("SDL could not be initialized: %s\n", SDL_GetError());
  else SDL_Log("SDL video system is ready to go!\n");

  SDL_Quit();
  
  return 0;
}
