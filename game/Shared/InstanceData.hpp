//
//  InstanceData.h
//  iOS
//
//  Created by Dmitrii Belousov on 9/3/22.
//

#ifndef InstanceData_h
#define InstanceData_h

struct InstanceData {
  glm::mat4x4 instanceTransform;
  uint16_t textureIndex;
  bool shouldFlip;
  // Padding to ensure that sizeof(InstanceData) returns the size of the struct that we allocated memory for
  char pad[11];
};

#endif /* InstanceData_h */
