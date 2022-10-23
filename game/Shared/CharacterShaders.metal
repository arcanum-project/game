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
  const uint squareSide = texture.get_width() > texture.get_height() ? texture.get_height() : texture.get_width();
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
  const float u = squareSide * (pointIndexX / (numPointsPerRow - 1));
  // Get v-coordinate from Z index
  const float v = squareSide * (pointIndexZ / (numPointsPerColumn - 1));
  const float centerX = 13.0f;
  const float centerY = 79.0f;
  float4x4 translateCenter = float4x4(1.0f);
  translateCenter[3][0] = centerX - squareSide / 2.0f;
  translateCenter[3][1] = centerY - squareSide / 2.0f;
  // Next step is to scale unwrapped UVs to ensure that entire image is covered
  float newWidthOrHeight = texture.get_width() > texture.get_height() ? texture.get_width() : texture.get_height();
  // world scaling factor is to scale the texture to be of an appropriate size relative to the game world
  const float worldScaleFactor = 2.5f;
  // uv scaling factor is to scale UVs to cover entire image
  const float uvScalingFactor = (newWidthOrHeight - centerX) / (squareSide / 2);
  const float scaleFactor = worldScaleFactor * uvScalingFactor;
  float4x4 scale = float4x4(1.0f);
  scale[0][0] = scaleFactor;
  scale[1][1] = scaleFactor;
  // When we scale our UVs, the center of the UVs will be scaled proportionally, too. However we don't want that - we want the UV center (0.5; 0.5) to be in the same place where it was before any scaling
  // To achieve this, we need to move (translate) the center back to original position after scaling.
  const float translateFactorX = centerX * (scaleFactor - 1);
  const float translateFactorY = centerY * (scaleFactor - 1);
  float4x4 translate = float4x4(1.0f);
  // translateFactor ensures that center is moved back to its original position before scaling.
  translate[3][0] = -translateFactorX;
  translate[3][1] = -translateFactorY;
  // Perform actual transformation
  float4 adjustedUV = translate * scale * translateCenter * float4(u, v, .0f, 1.0f);
  // Rotation is required to properly position the texture in the game world - i.e. to take into account the rotation of the game world itself
  float4x4 rotateY = float4x4(1.0f);
  const float angle = degreesToRadians(135.0f);
  rotateY[0][0] = cos(angle);
  // Rotate the texture around its desired center while preserving the texture's aspect ratio
  // Based on: https://blender.stackexchange.com/questions/213318/how-to-rotate-uv-and-preserve-the-correct-aspect-ratio
  const float aspectRatio = (float) texture.get_height() / (float) texture.get_width();
  rotateY[0][1] = texture.get_width() > texture.get_height() ? sin(angle) : sin(angle) / aspectRatio;
  rotateY[1][0] = -sin(angle);
  rotateY[1][1] = cos(angle);
  // Since we need to rotate the texture around the point that is different from texture's origin (origin is top left corner), we need to translate the origin to the point of rotation
  // From here: https://www.youtube.com/watch?v=nu2MR1RoFsA
  float4x4 translateBeforeRotate = float4x4(1.0f);
  translateBeforeRotate[3][0] = -centerX;
  translateBeforeRotate[3][1] = -centerY;
  float4x4 translateAfterRotate = float4x4(1.0f);
  translateAfterRotate[3][0] = centerX;
  translateAfterRotate[3][1] = centerY;
  adjustedUV = translateAfterRotate * rotateY * translateBeforeRotate * adjustedUV;
  // Normalize uvs to be in (0, 1) range
  adjustedUV.x /= texture.get_width();
  adjustedUV.y /= texture.get_height();
  const bool isUVOutsideYBounds = adjustedUV.y > 1.0f || adjustedUV.y < 0.0f;
  const bool isUVOutsideXBounds = adjustedUV.x > 1.0f || adjustedUV.x < 0.0f;
  // Since our mesh is larger than actual texture, we need to make areas outside of texture transparent
  if (isUVOutsideXBounds || isUVOutsideYBounds)
	return float4(0.0f, 0.0f, 0.0f, 0.0f);
  const half4 color = texture.sample(textureSampler, adjustedUV.xy);
  // Below is an ugly way to mask away blue texture background
  if (color.r < 0.16f && color.g < 0.16f && color.b > 0.3f)
	discard_fragment();
  return float4(color);
}
