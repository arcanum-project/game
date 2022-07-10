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
  float4 color [[attribute(1)]];
};

struct VertexOut
{
  float4 position [[position]];
  float4 color;
  float pointSize [[point_size]];
};

vertex VertexOut vertex_main(
							 VertexIn in [[stage_in]],
							 constant Uniforms &uniforms [[buffer(11)]]
							 )
{
  VertexOut out
  {
	.position = uniforms.projectionMatrix * uniforms.viewMatrix * uniforms.modelMatrix * in.position,
	.color = in.color,
	.pointSize = 30
  };
  return out;
}

fragment float4 fragment_main(VertexOut in [[stage_in]])
{
  return in.color;
}
