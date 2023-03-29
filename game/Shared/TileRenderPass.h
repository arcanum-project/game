//

#pragma once

#include <vector>
#include <Metal/MTLRenderPipeline.hpp>
#include <QuartzCore/CAMetalDrawable.hpp>

#include "GameScene.hpp"

class TileRenderPass
{
public:
  TileRenderPass(MTL::Device* device, MTL::Library* library, MTL::Buffer* materialBuffer, const uint16_t instanceCount, const uint16_t maxBuffersInFlight, GameScene* scene);
  ~TileRenderPass();
  
  void draw(MTL::CommandBuffer* commandBuffer, CA::MetalDrawable* drawable, MTL::Texture* depthTexture, GameScene* scene, float_t deltaTime, const uint16_t frame);
  
private:
  MTL::Device* device;
  MTL::RenderPipelineState* renderPipelineState;
  MTL::Buffer* materialBuffer;
  MTL::DepthStencilState* depthStencilState;
  MTL::IndirectCommandBuffer* indirectCommandBuffer;
  MTL::Buffer* icbArgumentBuffer;
  MTL::Function* tileVisibilityKernelFn;
  MTL::ComputePipelineState* computePipelineState;
  
  std::vector<MTL::Buffer*> instanceDataBuffers;
  MTL::Buffer* flippedVertexBuffer;
  MTL::Buffer* vertexBuffer;
  MTL::Buffer* indexBuffer;
  
  void buildPipelineStates(MTL::Library* library);
  void buildVertexBuffers(GameScene* scene);
  void buildDepthStencilState();
  void buildIndirectCommandBuffer();
};
