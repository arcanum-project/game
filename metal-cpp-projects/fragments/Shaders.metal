//
//  Shaders.metal
//  cameras
//
//  Created by Dmitrii Belousov on 7/3/22.
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

struct VertexIn
{
  float4 position [[attribute(0)]];
  float3 normal [[attribute(2)]];
};

struct VertexOut
{
  float4 position [[position]];
  float3 normal;
};

vertex VertexOut vertex_main(
							 VertexIn in [[stage_in]],
							 constant Uniforms &uniforms [[buffer(11)]]
							 )
{
  VertexOut out
  {
	.position = uniforms.projectionMatrix * uniforms.viewMatrix * uniforms.modelMatrix * in.position,
	.normal = in.normal
  };
  return out;
}

fragment float4 fragment_main(VertexOut in [[stage_in]])
{
  return float4(in.normal, 1);
}
