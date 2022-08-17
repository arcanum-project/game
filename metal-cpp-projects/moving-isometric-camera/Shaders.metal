//
//  Shaders.metal
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 7/24/22.
//

#include <metal_stdlib>
#include <metal_matrix>
using namespace metal;

typedef struct {
  float4x4 modelMatrix;
  float4x4 viewMatrix;
  float4x4 projectionMatrix;
} Uniforms;

enum BufferIndices {
  VertexBuffer = 0,
  TextureBuffer = 0,
  InstanceDataBuffer = 1,
  IndexBuffer = 2,
  UniformsBuffer = 11,
  ICBBuffer = 16,
  ArgumentsBuffer = 17
};
  
struct ICBContainer {
  command_buffer commandBuffer [[id(BufferIndices::ArgumentsBuffer)]];
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
  
kernel void cullTilesAndEncodeCommands(uint tileIndex [[thread_position_in_grid]],
									   constant Uniforms & uniforms [[buffer(BufferIndices::UniformsBuffer)]],
									   device const void * vertices [[buffer(BufferIndices::VertexBuffer)]],
									   device const ushort * indices [[buffer(BufferIndices::IndexBuffer)]],
									   device const InstanceData * instanceData [[buffer(BufferIndices::InstanceDataBuffer)]],
									   device const ICBContainer * pIcbContainer [[buffer(BufferIndices::ICBBuffer)]]) {
  const float4 tileCenter = instanceData[tileIndex].instanceTransform[3];
  const float4 projectedTileCenterPosition = uniforms.projectionMatrix * uniforms.viewMatrix * uniforms.modelMatrix * tileCenter;
  const float2 boundingRadius = float2(1.0f, 1.0f);
  // We divide by w to convert clip coordinates to actual NDC coordinates. Normally this will be done for us under the hood, but here we need to do it ourselves to check for visibility
  const bool isOutsideRightBounds = (projectedTileCenterPosition.x + boundingRadius.x) / projectedTileCenterPosition.w > 1.0f ? true : false;
  const bool isOutsideLeftBounds = (projectedTileCenterPosition.x - boundingRadius.x) / projectedTileCenterPosition.w < -1.0f ? true : false;
  const bool isOutsideLowerBounds = (projectedTileCenterPosition.y - boundingRadius.y) / projectedTileCenterPosition.w < -1.0f ? true : false;
  const bool isOutsideUpperBounds = (projectedTileCenterPosition.y + boundingRadius.y) / projectedTileCenterPosition.w > 1.0f ? true : false;
  bool isVisible = true;
  if (isOutsideLeftBounds || isOutsideRightBounds || isOutsideLowerBounds || isOutsideUpperBounds) {
	isVisible = false;
  }
  
  // Get indirect render command object from the indirect command buffer given the tile's unique
  // index to set parameters for drawing (or not drawing) it.
  render_command cmd(pIcbContainer->commandBuffer, tileIndex);
  
  if (isVisible) {
	cmd.set_vertex_buffer(& uniforms, BufferIndices::UniformsBuffer);
	cmd.set_vertex_buffer(vertices, BufferIndices::VertexBuffer);
	cmd.set_vertex_buffer(instanceData, BufferIndices::InstanceDataBuffer);
	cmd.draw_indexed_primitives(primitive_type::triangle, 6, indices, 1, 0, tileIndex);
  }
}

vertex VertexOut vertex_main(
							 VertexIn in [[stage_in]],
							 constant Uniforms & uniforms [[buffer(BufferIndices::UniformsBuffer)]],
							 device const InstanceData * instanceData [[buffer(BufferIndices::InstanceDataBuffer)]],
							 uint instanceId [[base_instance]]
							 )
{
  VertexOut out {
	.position = uniforms.projectionMatrix * uniforms.viewMatrix * uniforms.modelMatrix * instanceData[instanceId].instanceTransform * in.position,
	.texture = in.texture,
	.normal = in.normal
  };
  return out;
}

fragment float4 fragment_main(VertexOut in [[stage_in]]
//							  texture2d<half, access::sample> colorTexture [[texture(BufferIndices::TextureBuffer)]]
							  ) {
//  constexpr sampler textureSampler;

  // Sample the texture to obtain a color
//  const half4 colorSample = colorTexture.sample(textureSampler, in.texture);
//  return float4(colorSample);
  return float4(in.normal, 1);
//  return float4(0, 1, 0, 1);
}
