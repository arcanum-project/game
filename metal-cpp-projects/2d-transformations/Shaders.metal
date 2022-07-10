//
//  Shaders.metal
//  2d-transformations
//
//  Created by Dmitrii Belousov on 7/3/22.
//

#include <metal_stdlib>
using namespace metal;

struct VertexIn
{
  float4 position [[attribute(0)]];
  float4 color [[attribute(1)]];
};

struct VertexOut
{
  float4 position [[position]];
  float4 color;
};

vertex VertexOut vertex_main(
							 VertexIn in [[stage_in]],
							 constant float4x4 &translation [[buffer(11)]],
							 constant float4x4 &scaling [[buffer(12)]],
							 constant float4x4 &rotation [[buffer(13)]],
							 constant float4x4 &translationInverse [[buffer(14)]]
						  )
{
  VertexOut out
  {
//	.position = in.position,
//	.position = translation * in.position,
//	.position = scaling * in.position,
//	.position = rotation * in.position,
//	.position = translation * rotation * scaling * translationInverse * in.position,
//	.position = translationInverse * in.position,
	.position = translation * rotation * scaling * translationInverse * in.position,
	.color = in.color
  };
  return out;
}

fragment float4 fragment_main(VertexOut in [[stage_in]])
{
  return in.color;
}
