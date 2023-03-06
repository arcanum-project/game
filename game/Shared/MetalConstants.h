//
//  Constants.h
//  iOS
//
//  Created by Dmitrii Belousov on 9/3/22.
//

#ifndef Constants_h
#define Constants_h

typedef enum {
  VertexCoordinates = 0,
  TextureCoordinates = 1,
  NormalCoordinates = 2
} Attributes;

typedef enum {
  VertexBuffer = 0,
  InstanceDataBuffer = 1,
  IndexBuffer = 2,
  FlippedVertexBuffer = 3,
  TextureBuffer = 5,
  UniformsBuffer = 11,
  ICBBuffer = 16,
  ICBArgumentsBuffer = 17,
  RenderingMetadataBuffer = 19
} BufferIndices;

#endif /* Constants_h */
