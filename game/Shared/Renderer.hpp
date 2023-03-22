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

class Renderer
{
public:
  Renderer(MTL::Device * const _pDevice);
  ~Renderer();
  void drawFrame(const CA::MetalDrawable * const pDrawable, const MTL::Texture * const pDepthTexture);
  void drawableSizeWillChange(const float_t & drawableWidth, const float_t & drawableHeight);

private:
  MTL::Device * const _pDevice;
  MTL::CommandQueue * const _pCommandQueue;
  MTL::Library * const _pLib;
  MTL::RenderPipelineState * _pTilesPSO;
  MTL::RenderPipelineState * _pCritterPSO;
  MTL::DepthStencilState * _pDepthStencilState;
  MTL::ComputePipelineState * _pTilesComputePSO;
  MTL::IndirectCommandBuffer * _pIndirectCommandBuffer;
  MTL::Buffer * _pIcbArgumentBuffer;
  MTL::Function * _pTileVisibilityKernelFn;
  MTL::Buffer * _pModelsBuffer;
  float_t _angle;
  GameScene * const _pGameScene;
  uint16_t _frame;
  dispatch_semaphore_t _semaphore;
  std::chrono::time_point<std::chrono::system_clock> _lastTimeSeconds;
	
  void buildTileShaders();
  void buildCharacterShaders();
  void buildDepthStencilState();
  void initializeTextures();
};

#endif /* Renderer_hpp */
