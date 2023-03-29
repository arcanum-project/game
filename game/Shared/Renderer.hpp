//  Created by Dmitrii Belousov on 7/24/22.
//

#pragma once

#include <Metal/MTLDevice.hpp>
#include <Metal/MTLTexture.hpp>
#include <QuartzCore/CAMetalDrawable.hpp>
#include <chrono>

#include "GameScene.hpp"
#include "TileRenderPass.h"
#include "SpriteRenderPass.h"
#include "TextureController.hpp"

class Renderer
{
public:
  Renderer(MTL::Device* device);
  ~Renderer();
  void drawFrame(CA::MetalDrawable* drawable, MTL::Texture* depthTexture);
  void drawableSizeWillChange(const float_t drawableWidth, const float_t drawableHeight);

private:
  MTL::Device* device;
  MTL::CommandQueue* commandQueue;
  MTL::Library* library;
  MTL::Buffer* materialBuffer;
  GameScene* gameScene;
  uint16_t frame;
  dispatch_semaphore_t semaphore;
  std::chrono::time_point<std::chrono::system_clock> lastTimeSeconds;
  TileRenderPass* tileRenderPass;
  SpriteRenderPass* spriteRenderPass;
  
  void initializeTextures();
};
