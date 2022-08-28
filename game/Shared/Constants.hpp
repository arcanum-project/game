//
//  Constants.h
//  metal-cpp-projects
//
//  Created by Dmitrii Belousov on 7/25/22.
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
  ModelsBuffer = 5,
  UniformsBuffer = 11,
  ICBBuffer = 16,
  ICBArgumentsBuffer = 17
} BufferIndices;

typedef enum {
  MaxBuffersInFlight = 3,
  NumOfTilesPerSector = 4096,
  NumOfTilesPerRow = 64
} RenderingConstants;

#endif /* Constants_h */
