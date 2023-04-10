//

#pragma once

#include <vector>
#include <Metal/MTLRenderPipeline.hpp>
#include <Metal/MTLCommandBuffer.hpp>
#include <Metal/MTLComputePipeline.hpp>
#include <QuartzCore/CAMetalDrawable.hpp>

#include "GameScene.hpp"

class TileRenderPass
{
public:
  TileRenderPass(MTL::Device* device, MTL::Library* library, MTL::Buffer* const& materialBuffer, const uint16_t instanceCount, const uint16_t maxBuffersInFlight, GameScene* scene);
  ~TileRenderPass();
  
  void draw(MTL::CommandBuffer* commandBuffer, CA::MetalDrawable* drawable, MTL::Texture* depthTexture, GameScene* scene, float_t deltaTime, const uint16_t frame);
  
private:
  MTL::Device* device;
  MTL::RenderPipelineState* renderPipelineState;
  // Has to be pointer reference, because actual pointer will be reassigned after constructor of this class is called
  MTL::Buffer* const& materialBuffer;
  MTL::DepthStencilState* depthStencilState;
  MTL::IndirectCommandBuffer* indirectCommandBuffer;
  MTL::Buffer* icbArgumentBuffer;
  MTL::Function* tileVisibilityKernelFn;
  MTL::ComputePipelineState* computePipelineState;
  
  std::vector<MTL::Buffer*> instanceDataBuffers;
  MTL::Buffer* flippedVertexBuffer;
  MTL::Buffer* vertexBuffer;
  MTL::Buffer* indexBuffer;
  std::vector<MTL::Buffer*> uniformsBuffers;
  
  std::unordered_map<uint16_t, TileInstanceData> instanceIdToData;
  
  void buildPipelineStates(MTL::Library* library);
  void buildVertexBuffers(GameScene* scene);
  void buildDepthStencilState();
  void buildIndirectCommandBuffer();
  void loadTextures(GameScene* scene);
  const uint16_t makeTexturesFromArt(const char * name, const char * type) const;
};
