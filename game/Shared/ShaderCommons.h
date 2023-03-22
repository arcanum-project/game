//
//  Common.h
//  metal-cpp-projects
//
//  Created by Dmitrii Belousov on 7/25/22.
//

#pragma once

#include <metal_stdlib>

using namespace metal;

typedef struct {
  float4x4 modelMatrix;
  float4x4 viewMatrix;
  float4x4 projectionMatrix;
  float drawableWidth;
  float drawableHeight;
} Uniforms;

typedef struct {
  metal::array<texture2d<half, access::sample>, 4096> baseColorTextures;
} ShaderMaterial;


inline float2 worldToScreen(const float4x4 projectionMatrix, const float4x4 viewMatrix, const float drawableWidth, const float drawableHeight, const float4 coordinateWorld)
{
  const float4 clip = projectionMatrix * viewMatrix * coordinateWorld;
  const float viewportX = (((clip.x / clip.w) + 1) / 2) * drawableWidth;
  const float viewportY = (((-clip.y / clip.w) + 1) / 2) * drawableHeight;
  return float2(viewportX, viewportY);
}

inline float2 screenToNDC(const float2 coordinateScreen, const float drawableWidth, const float drawableHeight)
{
  float4 vec4 = float4(coordinateScreen, 0.f, 1.f);
  const float scaleX = 2.0f / drawableWidth;
  const float scaleY = -2.0f / drawableHeight;
  const float4x4 scaleMat = float4x4({ scaleX, .0f, .0f, .0f }, { .0f, scaleY, .0f, .0f }, { .0f, .0f, .0f, .0f }, { -1.0f, 1.0f, .0f, .0f });
  const float4 ndc = scaleMat * vec4;
  return float2(ndc.x, ndc.y);
}
