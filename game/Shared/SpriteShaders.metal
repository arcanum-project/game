//

#include <metal_stdlib>
using namespace metal;

#include "MetalConstants.h"
#include "ShaderCommons.h"

struct VertexOut
{
  float4 position [[position]];
  float2 uv;
};

/**
 This shader creates rectangular mesh of the same dimensions as the sprite texture. We want it that way to avoid any sprite scaling during sampling to preserve original sprite size.
 This behavior closely matches what the original engine does, too.
 */
vertex VertexOut spriteVS(constant const packed_float3* tileCenterWorld [[buffer(BufferIndices::VertexBuffer)]],
						  constant const int* renderingMetadata [[buffer(BufferIndices::RenderingMetadataBuffer)]],
						  constant const Uniforms& uniforms [[buffer(BufferIndices::UniformsBuffer)]],
						  const unsigned short index [[vertex_id]])
{
  // We know the center coordinates of a tile to place the sprite at in advance
  float2 tileCenterScreen = worldToScreen(uniforms.projectionMatrix, uniforms.viewMatrix, uniforms.drawableWidth, uniforms.drawableHeight, float4(tileCenterWorld[0], 1.f));
  const float textureWidth = renderingMetadata[3];
  const float textureHeight = renderingMetadata[4];
  // Sprite center coordinates come from original sprite metadata
  const float2 spriteCenterTextureSpace = float2(renderingMetadata[1], renderingMetadata[2]);
  
  // Knowing tile center coordinates in screen space and the sprite dimensions, we can calculate coordinates of each of the four corners of the quad mesh
  // Sprite center must be at the tile center.
  const float2 topLeftScreen = float2(tileCenterScreen.x - spriteCenterTextureSpace.x, tileCenterScreen.y - spriteCenterTextureSpace.y);
  const float2 topRightScreen = float2(tileCenterScreen.x + textureWidth - spriteCenterTextureSpace.x, tileCenterScreen.y - spriteCenterTextureSpace.y);
  const float2 bottomLeftScreen = float2(tileCenterScreen.x - spriteCenterTextureSpace.x, tileCenterScreen.y + textureHeight - spriteCenterTextureSpace.y);
  const float2 bottomRightScreen = float2(tileCenterScreen.x + textureWidth - spriteCenterTextureSpace.x, tileCenterScreen.y + textureHeight - spriteCenterTextureSpace.y);
  
  // Vertex shader in Metal is supposed to return vertex coordinates in clip space, therefore we need to convert from screen to clip space
  float2 positionsNDC[4];
  positionsNDC[0] = screenToNDC(topLeftScreen, uniforms.drawableWidth, uniforms.drawableHeight);
  positionsNDC[1] = screenToNDC(topRightScreen, uniforms.drawableWidth, uniforms.drawableHeight);
  positionsNDC[2] = screenToNDC(bottomLeftScreen, uniforms.drawableWidth, uniforms.drawableHeight);
  positionsNDC[3] = screenToNDC(bottomRightScreen, uniforms.drawableWidth, uniforms.drawableHeight);
  
  // Now that the mesh to sample our sprite to is generated, we also need to define uv coordinates for it - so that texture sampler can use them
  float2 uvs[4];
  uvs[0] = float2(0.f, 0.f);
  uvs[1] = float2(1.f, 0.f);
  uvs[2] = float2(0.f, 1.f);
  uvs[3] = float2(1.f, 1.f);
  
  VertexOut out
  {
	.position = float4(positionsNDC[index], 0.f, 1.f),
	.uv = uvs[index]
  };
  return out;
}

fragment half4 spriteFS(const VertexOut in [[stage_in]], constant const ShaderMaterial& material [[buffer(BufferIndices::TextureBuffer)]],
						 constant const int* renderingMetadata [[buffer(BufferIndices::RenderingMetadataBuffer)]])
{
  constexpr sampler textureSampler;
  // Get proper texture from heap
  const texture2d<half, access::sample> texture = material.baseColorTextures[renderingMetadata[0]];
  const half4 color = texture.sample(textureSampler, in.uv);
  // Below is an ugly way to mask away blue texture background
  if (color.r < 0.16f && color.g < 0.16f && color.b > 0.3f)
	// We could simply return transparent color here, but in that case Metal will still store depth values for this pixel.
	// We don't need that, therefore discard_fragment is a preferred option
	discard_fragment();
  return color;
}
