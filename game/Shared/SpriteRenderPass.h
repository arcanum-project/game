//

#pragma once

#include <Metal/MTLRenderPipeline.hpp>
#include <QuartzCore/CAMetalDrawable.hpp>

#include "GameScene.hpp"
#include "SpriteInstanceData.h"

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
  
  // Subset of loaded assets data to be passed to GPU to render the current frame
  struct RenderingMetadata
  {
	uint32_t currentTextureIndex;
	int32_t currentFrameCenterX;
	int32_t currentFrameCenterY;
	uint32_t currentTextureWidth;
	uint32_t currentTextureHeight;
  };
  RenderingMetadata renderingMetadata;
  
  void createUniformsBuffer();
};
