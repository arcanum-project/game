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

vertex VertexOut characterVertex(
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

fragment float4 characterFragment(VertexOut in [[stage_in]],
								constant ShaderMaterial & material [[buffer(BufferIndices::TextureBuffer)]],
								constant ushort & textureIndex [[buffer(19)]]) {
  constexpr sampler textureSampler(filter::linear, max_anisotropy(16));
  // Get proper texture from heap
  const texture2d<half, access::sample> texture = material.baseColorTextures[textureIndex];
  // In order to make textures aligned to the center-ish of a mesh, we calculate uvs ourself
  // Mesh is a square of 5x5 tiles, and we unwrap uvs to initially be a square aligned to the left-bottom corner of the texture
  // Side of this square will match either width or height of the texture, whichever is lowest - we do this to ensure that square spans entire side of the texture - again, that would be either width or height
  const uint max = texture.get_width() > texture.get_height() ? texture.get_height() : texture.get_width();
  // Number of vertices along the X axis of the mesh
  const ushort numPointsPerRow = 6;
  // Number of vertices along the Y axis of the mesh
  const ushort numPointsPerColumn = 6;
  // This is from Blender internal coordinates directly. These coordinates define mesh dimensions
  const short xMin = -5;
  const short zMax = 5;
  // Fragment shader receives coordinates of a pixel within a mesh. Below we identify 'index' of this coordinate.
  // Index can be thought of as relative position of a pixel on the mesh
  const float pointIndexX = (in.originalPosition.x - xMin) / 2;
  const float pointIndexZ = (zMax - in.originalPosition.z) / 2;
  // Get u-coordinate from X index
  const float u = max * (pointIndexX / (numPointsPerRow - 1));
  // Get v-coordinate from Z index
  const float v = max * (pointIndexZ / (numPointsPerColumn - 1));
  // Next step is to scale unwrapped UVs to ensure that entire image is covered
  const float newWidthOrHeight = texture.get_width() > texture.get_height() ? texture.get_width() : texture.get_height();
  // scaleFactor = world scaling factor * uv scaling factor.
  // uv scaling factor = (2 * newWidthOrHeight - max) / max. This actually scales UVs to cover entire image
  // world scaling factor = 2.5f. This scales the texture to be of an appropriate size relative to the game world
  const float scaleFactor = 2.5f * (2 * newWidthOrHeight - max) / max;
  float4x4 scale = float4x4(1.0f);
  scale[0][0] = scaleFactor;
  scale[1][1] = scaleFactor;
  // When we scale our UVs, the center of the UVs will be scaled proportionally, too. However we don't want that - we want the UV center (0.5; 0.5) to be in the same place where it was before any scaling
  // To achieve this, we need to move (translate) the center back to original position after scaling.
  const float translateFactor = max / 2 * (scaleFactor - 1);
  float4x4 translate = float4x4(1.0f);
  const float aspectRatio = texture.get_width() > texture.get_height() ? (float) texture.get_width() / (float) texture.get_height() : (float) texture.get_height() / (float) texture.get_width();
  // Second summand = max / 6 - has nothing to do with translating the center back to original position. This summand's purpose is to more accurately align the texture to the center of the mesh.
  translate[3][0] = aspectRatio >= 1.7 ? -translateFactor - max / 4 : -translateFactor - max / 6;
  translate[3][1] = aspectRatio >= 1.7 ? -translateFactor + max / 4 : -translateFactor - max / 6;
  // Rotation is required to properly position the texture in the game world - i.e. to take into account the rotation of the game world itself
  float4x4 rotateY = float4x4(1.0f);
  const float angle = degreesToRadians(-45.0f);
  rotateY[0][0] = cos(angle);
  // Rotate the texture preserving its aspect ratio
  // From here: https://blender.stackexchange.com/questions/213318/how-to-rotate-uv-and-preserve-the-correct-aspect-ratio
  rotateY[0][1] = sin(angle) / aspectRatio;
  rotateY[1][0] = -sin(angle);
  rotateY[1][1] = cos(angle);
  // Perform actual transformation
  float4 adjustedUV = rotateY * translate * scale * float4(u, v, .0f, 1.0f);
  // Normalize uvs to be in (0, 1) range
  adjustedUV.x /= texture.get_width();
  adjustedUV.y /= texture.get_height();
  // Mirror texture across X axis. This is also required to properly position the texture in the world
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
