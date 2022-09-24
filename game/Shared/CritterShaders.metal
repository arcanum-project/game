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

constexpr float degreesToRadians(float degrees) { return M_PI_H * degrees / 180; }

struct VertexIn {
  float4 position [[attribute(Attributes::VertexCoordinates)]];
  float2 texture [[attribute(Attributes::TextureCoordinates)]];
  float3 normal [[attribute(Attributes::NormalCoordinates)]];
};

struct VertexOut {
  float4 position [[position]];
  float4 originalPosition;
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
	.originalPosition = in.position,
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
  const uint max = texture.get_width() > texture.get_height() ? texture.get_height() : texture.get_width();
  const ushort numPointsPerRow = 4;
  const ushort numPointsPerColumn = 4;
  const short xMin = -3;
  const short zMax = 3;
  const float pointIndexX = (in.originalPosition.x - xMin) / 2;
  const float pointIndexZ = (zMax - in.originalPosition.z) / 2;
  const float u = max * (pointIndexX / (numPointsPerRow - 1));
  const float v = max * (pointIndexZ / (numPointsPerColumn - 1));
  const float newWidthOrHeight = texture.get_width() > texture.get_height() ? texture.get_width() : texture.get_height();
  const float scaleFactor = 1.5 * (2 * newWidthOrHeight - max) / max;
  float4x4 scale = float4x4(1.0f);
  scale[0][0] = scaleFactor;
  scale[1][1] = scaleFactor;
  const float translateFactor = max / 2 * (scaleFactor - 1);
  float4x4 translate = float4x4(1.0f);
  translate[3][0] = -translateFactor;
  translate[3][1] = -translateFactor;
  float4x4 rotateY = float4x4(1.0f);
  const float angle = degreesToRadians(-45.0f);
  rotateY[0][0] = cos(angle);
  const float aspectRatio = texture.get_height() / texture.get_width();
  // Rotate the texture preserving its aspect ratio
  // From here: https://blender.stackexchange.com/questions/213318/how-to-rotate-uv-and-preserve-the-correct-aspect-ratio
  rotateY[0][1] = sin(angle) / aspectRatio;
  rotateY[1][0] = -sin(angle);
  rotateY[1][1] = cos(angle);
  float4 adjustedUV = rotateY * translate * scale * float4(u, v, .0f, 1.0f);
  // Normalize uvs to be in (0, 1) range
  adjustedUV.x /= texture.get_width();
  adjustedUV.y /= texture.get_height();
  // Mirror texture across X axis
  adjustedUV.x = -adjustedUV.x + 1.0f;
  const bool isUVOutsideYBounds = adjustedUV.y > 1.0f || adjustedUV.y < 0.0f;
  const bool isUVOutsideXBounds = adjustedUV.x > 1.0f || adjustedUV.x < 0.0f;
  // Since our mesh is larger than actual texture, we need to make areas outside of texture transparent
  if (isUVOutsideXBounds || isUVOutsideYBounds)
	return float4(0.0f, 0.0f, 0.0f, 0.0f);
  const half4 color = texture.sample(textureSampler, adjustedUV.xy);
  // Below is an ugly way to mask away blue texture background
  return color.r < 0.16f && color.g < 0.16f && color.b > 0.3f ? float4(0.0f, 0.0f, 0.0f, 0.0f) : float4(color);
}
