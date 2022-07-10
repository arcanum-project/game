//
//  Renderer.hpp
//  2d-transformations
//
//  Created by Dmitrii Belousov on 7/3/22.
//

#include "Metal/Metal.hpp"
#include <QuartzCore/CAMetalDrawable.hpp>

class Renderer
{
  public:
	Renderer(MTL::Device * const _pDevice);
	~Renderer();
	void drawFrame(const CA::MetalDrawable * const pDrawable);

  private:
	MTL::Device * const _pDevice;
	MTL::CommandQueue * const _pCommandQueue;
	MTL::RenderPipelineState * _pPSO;
	void buildShaders();
};
