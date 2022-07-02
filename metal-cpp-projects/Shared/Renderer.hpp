//
//  Renderer.hpp
//  triangle-with-animation
//
//  Created by Dmitrii Belousov on 7/1/22.
//

#include "Metal/Metal.hpp"
#include <QuartzCore/CAMetalDrawable.hpp>

class Renderer
{
  public:
	Renderer(MTL::Device * const _pDevice);
	~Renderer();
	void draw(const CA::MetalDrawable * const pDrawable) const;

  private:
	MTL::Device * const _pDevice;
	MTL::CommandQueue * const _pCommandQueue;
};
