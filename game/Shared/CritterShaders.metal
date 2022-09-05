//
//  CritterShaders.metal
//  iOS
//
//  Created by Dmitrii Belousov on 9/3/22.
//

#include <metal_stdlib>

#include "Constants.h"
#include "ShaderStructs.h"

using namespace metal;

struct VertexIn {
  float4 position [[attribute(Attributes::VertexCoordinates)]];
  float2 texture [[attribute(Attributes::TextureCoordinates)]];
  float3 normal [[attribute(Attributes::NormalCoordinates)]];
};

struct VertexOut {
  float4 position [[position]];
  float2 texture;
  float3 normal;
};

vertex VertexOut critterVertex(
							 VertexIn in [[stage_in]],
							 constant Uniforms & uniforms [[buffer(BufferIndices::UniformsBuffer)]]
							 )
{
  VertexOut out {
	.position = uniforms.projectionMatrix * uniforms.viewMatrix * uniforms.modelMatrix * in.position,
	.texture = in.texture,
	.normal = in.normal
  };
  return out;
}

fragment float4 critterFragment(VertexOut in [[stage_in]],
								constant ShaderMaterial & material [[buffer(BufferIndices::TextureBuffer)]],
								constant ushort & textureIndex [[buffer(19)]]) {
  constexpr sampler textureSampler(filter::linear, max_anisotropy(16));
  // Get proper texture from heap
  const texture2d<half, access::sample> texture = material.baseColorTextures[textureIndex];
  // identity matrix
  float4x4 scale = float4x4(1.0f);
  // Take into account texture's aspect ratio and make it 5x smaller
  // We multiply aspect ratio by a constant to closely match art style of original game
  scale[0][0] = 3.5f * (1.5f * texture.get_height() / texture.get_width());
  scale[1][1] = 3.5f;
  // Once we scaled UVs, we need to align the sprite to the middle-ish of its mesh
  float4x4 translate = float4x4(1.0f);
  translate[3][0] = -0.465f;
  translate[3][1] = -0.485f;
  const float4 adjustedUV = scale * translate * float4(in.texture.x, in.texture.y, .0f, 1.0f);
  const bool isUVOutsideYBounds = adjustedUV.y > 1.0f || adjustedUV.y < 0.0f;
  const bool isUVOutsideXBounds = adjustedUV.x > 1.0f || adjustedUV.x < 0.0f;
  // Since our mesh is larger than actual texture, we need to make areas outside of texture transparent
  if (isUVOutsideXBounds || isUVOutsideYBounds)
	return float4(0.0f, 0.0f, 0.0f, 0.0f);
  const half4 color = material.baseColorTextures[textureIndex].sample(textureSampler, adjustedUV.xy);
  // Below is an ugly way to mask away blue texture background
  return color.r < 0.16f && color.g < 0.16f && color.b > 0.3f ? float4(0.0f, 0.0f, 0.0f, 0.0f) : float4(color);
}
