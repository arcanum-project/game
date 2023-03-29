//
//  Renderer.hpp
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 7/24/22.
//

#ifndef Renderer_hpp
#define Renderer_hpp

#include "Metal/Metal.hpp"
#include <QuartzCore/CAMetalDrawable.hpp>
#include <array>
#include "glm/vec3.hpp"
#include <chrono>

#include "VertexDescriptor.hpp"
#include "Uniforms.hpp"
#include "Math.hpp"
#include "Transformable.hpp"
#include "GameScene.hpp"
#include "ObjModelImporter.hpp"
#include "TileRenderPass.h"
#include "SpriteRenderPass.h"

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

#endif /* Renderer_hpp */
