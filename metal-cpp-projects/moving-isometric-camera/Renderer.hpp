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
#include "VertexDescriptor.hpp"
#include <array>
#include "glm/vec3.hpp"
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
  MTL::RenderPipelineState * _pPSO;
  MTL::DepthStencilState * _pDepthStencilState;
  float_t _angle;
  GameScene * const _pGameScene;
	
  void buildShaders();
  void buildDepthStencilState();
};

#endif /* Renderer_hpp */
