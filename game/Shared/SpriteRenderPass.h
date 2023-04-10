//

#pragma once

#include <Metal/MTLRenderPipeline.hpp>
#include <Metal/MTLCommandBuffer.hpp>
#include <QuartzCore/CAMetalDrawable.hpp>

#include "GameScene.hpp"
#include "SpriteTextureData.h"

class SpriteRenderPass
{
public:
  SpriteRenderPass(MTL::Device* device, MTL::Library* library, MTL::Buffer* const& materialBuffer);
  ~SpriteRenderPass();
  
  inline const SpriteTextureData& getInstanceData() const { return textureData; }
  
  void buildDepthStencilState();
  
  void loadTextures();
  void makeTexturesFromArt(const char* name, const char* type, PixelData* const pixelDataOut, uint16_t& textureStartIndexOut);
  
  void draw(MTL::CommandBuffer* commandBuffer, CA::MetalDrawable* drawable, MTL::Texture* depthTexture, GameScene* scene, float_t deltaTime);
  
  void updateSpriteTexture(float_t deltaTime, Sprite* sprite);
  
private:
  MTL::Device* device;
  MTL::RenderPipelineState* pipelineState;
  // Has to be pointer reference, because actual pointer will be reassigned after constructor of this class is called
  MTL::Buffer* const& materialBuffer;
  MTL::DepthStencilState* depthStencilState;
  
  SpriteTextureData textureData;
  
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
};
