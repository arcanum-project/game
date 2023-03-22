//
//  Shaders.metal
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 7/24/22.
//

#include <metal_stdlib>
#include <metal_matrix>

#include "MetalConstants.h"
#include "ShaderCommons.h"

using namespace metal;
  
struct ICBContainer {
  command_buffer commandBuffer [[id(BufferIndices::ICBArgumentsBuffer)]];
};

struct VertexIn {
  float4 position [[attribute(Attributes::VertexCoordinates)]];
  float2 texture [[attribute(Attributes::TextureCoordinates)]];
  float3 normal [[attribute(Attributes::NormalCoordinates)]];
};

struct VertexOut {
  float4 position [[position]];
  float2 texture;
  float3 normal;
  ushort textureIndex;
};
  
typedef struct {
  float4x4 instanceTransform;
  ushort textureIndex;
  bool shouldFlip;
  // Padding to ensure that size of the struct is the same as we allocated memory for. This is crucial, because these structs are stored in an array
  char pad[13];
} InstanceData;
  
kernel void cullTilesAndEncodeCommands(uint tileIndex [[thread_position_in_grid]],
									   constant Uniforms & uniforms [[buffer(BufferIndices::UniformsBuffer)]],
									   device const void * vertices [[buffer(BufferIndices::VertexBuffer)]],
									   device const void * flippedVertices [[buffer(BufferIndices::FlippedVertexBuffer)]],
									   device const ushort * indices [[buffer(BufferIndices::IndexBuffer)]],
									   device const InstanceData * instanceData [[buffer(BufferIndices::InstanceDataBuffer)]],
									   device const ICBContainer * pIcbContainer [[buffer(BufferIndices::ICBBuffer)]],
									   constant ShaderMaterial & material [[buffer(BufferIndices::TextureBuffer)]]
									   ) {
  const float4 tileCenter = instanceData[tileIndex].instanceTransform[3];
  const float4 projectedTileCenterPosition = uniforms.projectionMatrix * uniforms.viewMatrix * uniforms.modelMatrix * tileCenter;
  /* if bounding radius = 0, it means we will render only the tiles visible on screen.
   However, it wil cause us to see tiles appear and disappear as we move camera.
   To prevent that, we want to render tiles we see on screen and a few more tiles beyond the screen to create the illusion of continuous world
  */
  const float2 boundingRadius = float2(-0.8f, -0.8f);
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
	if (instanceData[tileIndex].shouldFlip == true)
	  cmd.set_vertex_buffer(flippedVertices, BufferIndices::VertexBuffer);
	else
	  cmd.set_vertex_buffer(vertices, BufferIndices::VertexBuffer);
	cmd.set_vertex_buffer(flippedVertices, BufferIndices::FlippedVertexBuffer);
	cmd.set_vertex_buffer(instanceData, BufferIndices::InstanceDataBuffer);
	cmd.set_fragment_buffer(& material, BufferIndices::TextureBuffer);
	cmd.draw_indexed_primitives(primitive_type::triangle, 6, indices, 1, 0, tileIndex);
  }
}

vertex VertexOut tileVertex(
							 VertexIn in [[stage_in]],
							 constant Uniforms & uniforms [[buffer(BufferIndices::UniformsBuffer)]],
							 device const InstanceData * instanceData [[buffer(BufferIndices::InstanceDataBuffer)]],
							 uint instanceId [[base_instance]]
							 )
{
  VertexOut out {
	.position = uniforms.projectionMatrix * uniforms.viewMatrix * uniforms.modelMatrix * instanceData[instanceId].instanceTransform * in.position,
	.texture = in.texture,
	.normal = in.normal,
	.textureIndex = instanceData[instanceId].textureIndex
  };
  return out;
}

fragment float4 tileFragment(VertexOut in [[stage_in]],
							  constant ShaderMaterial & material [[buffer(BufferIndices::TextureBuffer)]]
							  ) {
  constexpr sampler textureSampler(filter::linear, max_anisotropy(16));

  // Sample the texture to obtain a color
  const half4 colorSample = material.baseColorTextures[in.textureIndex].sample(textureSampler, in.texture);
  return float4(colorSample);
}
