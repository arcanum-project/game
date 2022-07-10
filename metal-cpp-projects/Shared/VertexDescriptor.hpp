//
//  VertexDescriptor.hpp
//  triangle-with-animation
//
//  Created by Dmitrii Belousov on 7/2/22.
//

#pragma once

#include "Metal/Metal.hpp"
#include "glm/vec3.hpp"

class VertexDescriptor
{
  public:
	//  constructors
	static const VertexDescriptor & getInstance();
	// Tell compielr not to generate default copy constructors
	// From here: https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
	VertexDescriptor(VertexDescriptor const &) = delete;
	void operator=(VertexDescriptor const &) = delete;
	
	const MTL::VertexDescriptor * const getDefaultLayout() const;
  
  private:
	MTL::VertexDescriptor * _default;
	VertexDescriptor();
	~VertexDescriptor();
};
