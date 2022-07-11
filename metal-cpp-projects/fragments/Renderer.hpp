//
//  Renderer.hpp
//  fragments
//
//  Created by Dmitrii Belousov on 7/10/22.
//

#pragma once

#include "Metal/Metal.hpp"
#include <QuartzCore/CAMetalDrawable.hpp>
#include "VertexDescriptor.hpp"
#include <array>
#include "glm/vec3.hpp"
#include "Uniforms.hpp"
#include "Math.hpp"
#include "Transformable.hpp"
#include "../Shared/Model.hpp"
#include "ObjModelImporter.hpp"

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
	float_t _angle;
	Model * const _pModel;
	
	void buildShaders();
};
