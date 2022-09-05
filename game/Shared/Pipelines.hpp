//
//  Pipelines.h
//  iOS
//
//  Created by Dmitrii Belousov on 9/3/22.
//

#ifndef Pipelines_h
#define Pipelines_h

#include <Metal/Metal.hpp>

class Pipelines {
public:
  static MTL::RenderPipelineState * newPSO(MTL::Device * const pDevice, MTL::Library * const pLib, const NS::String * const vertexFnName, const NS::String * const fragmentFnName, const bool enableBlending);
  static MTL::ComputePipelineState * newComputePSO(MTL::Device * const pDevice, MTL::Library * const pLib, const NS::String * const fnName);
};

#endif /* Pipelines_h */
