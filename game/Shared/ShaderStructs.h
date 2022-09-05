//
//  Common.h
//  metal-cpp-projects
//
//  Created by Dmitrii Belousov on 7/25/22.
//

#ifndef Common_h
#define Common_h

#include <metal_stdlib>

using namespace metal;

typedef struct {
  float4x4 modelMatrix;
  float4x4 viewMatrix;
  float4x4 projectionMatrix;
} Uniforms;

typedef struct {
  metal::array<texture2d<half, access::sample>, 4096> baseColorTextures;
} ShaderMaterial;

#endif /* Common_h */
