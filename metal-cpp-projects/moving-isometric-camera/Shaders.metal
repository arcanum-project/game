//
//  Shaders.metal
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 7/24/22.
//

#include <metal_stdlib>
#include <metal_matrix>
using namespace metal;

struct Uniforms
{
  float4x4 modelMatrix;
  float4x4 viewMatrix;
  float4x4 projectionMatrix;
};

enum BufferIndices {
  TextureBuffer = 0,
  InstanceDataBuffer = 1,
  UniformsBuffer = 11
};
  
enum Attributes {
  VertexCoordinates = 0,
  TextureCoordinates = 1,
  NormalCoordinates = 2
};

struct VertexIn
{
  float4 position [[attribute(Attributes::VertexCoordinates)]];
  float2 texture [[attribute(Attributes::TextureCoordinates)]];
  float3 normal [[attribute(Attributes::NormalCoordinates)]];
};

struct VertexOut
{
  float4 position [[position]];
  float2 texture;
  float3 normal;
};
  
struct InstanceData {
  float4x4 instanceTransform;
};

vertex VertexOut vertex_main(
							 VertexIn in [[stage_in]],
							 constant Uniforms & uniforms [[buffer(BufferIndices::UniformsBuffer)]],
							 device const InstanceData * instanceData [[buffer(BufferIndices::InstanceDataBuffer)]],
							 uint instanceId [[instance_id]])
{
  float4 tileCenter = instanceData[instanceId].instanceTransform[3];
  const float4 projectedTileCenterPosition = uniforms.projectionMatrix * uniforms.viewMatrix * tileCenter;
  const float2 boundingRadius = float2(1.0f, 1.0f);
  const bool isOutsideRightBounds = (projectedTileCenterPosition.x + boundingRadius.x) / projectedTileCenterPosition.w > 1.0f ? true : false;
  const bool isOutsideLeftBounds = (projectedTileCenterPosition.x - boundingRadius.x) / projectedTileCenterPosition.w < -1.0f ? true : false;
  const bool isOutsideLowerBounds = (projectedTileCenterPosition.y - boundingRadius.y) / projectedTileCenterPosition.w < -1.0f ? true : false;
  const bool isOutsideUpperBounds = (projectedTileCenterPosition.y + boundingRadius.y) / projectedTileCenterPosition.w > 1.0f ? true : false;
  bool isVisible = true;
  if (isOutsideLeftBounds || isOutsideRightBounds || isOutsideLowerBounds || isOutsideUpperBounds) {
	isVisible = false;
  }
  VertexOut out {
	.position = uniforms.projectionMatrix * uniforms.viewMatrix * uniforms.modelMatrix * instanceData[instanceId].instanceTransform * in.position,
	.texture = in.texture,
	.normal = in.normal
  };
  return out;
}

fragment float4 fragment_main(VertexOut in [[stage_in]],
							  texture2d<half, access::sample> colorTexture [[texture(BufferIndices::TextureBuffer)]]) {
  constexpr sampler textureSampler;

  // Sample the texture to obtain a color
  const half4 colorSample = colorTexture.sample(textureSampler, in.texture);
  return float4(colorSample);
//  return float4(in.normal, 1);
}
