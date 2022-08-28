//
//  VertexData.hpp
//  fragments
//
//  Created by Dmitrii Belousov on 7/12/22.
//

#ifndef VertexData_hpp
#define VertexData_hpp

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

struct VertexData
{
  VertexData()
  : vertex(),
	texture(),
	normal() {}
  
  glm::vec3 vertex;
  glm::vec2 texture;
  glm::vec3 normal;
};

#endif /* VertexData_hpp */
