//  Created by Dmitrii Belousov on 7/24/22.
//

#pragma once

#include <SDL.h>
#include <Metal/MTLDevice.hpp>
#include <Metal/MTLTexture.hpp>
#include <QuartzCore/CAMetalLayer.hpp>
#include <chrono>

#include "GameScene.hpp"
#include "TileRenderPass.h"
#include "SpriteRenderPass.h"
#include "TextureController.hpp"

class Renderer
{
public:
  Renderer(SDL_Window* window);
  ~Renderer();
  void drawFrame();
  void drawableSizeWillChange(const float_t drawableWidth, const float_t drawableHeight);

private:
  SDL_Window* window;
  SDL_Renderer* sdlRenderer;
  CA::MetalLayer* layer;
  MTL::Device* device;
  MTL::CommandQueue* commandQueue;
  MTL::Library* library;
  MTL::Texture* depthTexture;
  MTL::Buffer* materialBuffer;
  GameScene* gameScene;
  uint16_t frame;
  dispatch_semaphore_t semaphore;
  std::chrono::time_point<std::chrono::system_clock> lastTimeSeconds;
  TileRenderPass* tileRenderPass;
  SpriteRenderPass* spriteRenderPass;
  
  SDL_Renderer* createSDLRenderer();
  void initializeTextures();
  void createDepthTexture(float_t drawableWidth, float_t drawableHeight);
};
