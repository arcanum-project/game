//

#pragma once

#include <Metal/MTLRenderPipeline.hpp>
#include <QuartzCore/CAMetalDrawable.hpp>

#include "GameScene.hpp"

class SpriteRenderPass
{
public:
  SpriteRenderPass(MTL::Device* device, MTL::Library* library, MTL::Buffer* textureBuffer);
  ~SpriteRenderPass();
  
  void buildDepthStencilState();
  void draw(MTL::CommandBuffer* commandBuffer, CA::MetalDrawable* drawable, MTL::Texture* depthTexture, GameScene* scene, float_t deltaTime);
  
private:
  MTL::Device* device;
  MTL::RenderPipelineState* pipelineState;
  MTL::Buffer* textureBuffer;
  MTL::DepthStencilState* depthStencilState;
};
