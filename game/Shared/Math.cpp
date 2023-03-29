//
//  Math.cpp
//  cameras
//
//  Created by Dmitrii Belousov on 7/3/22.
//

#include "Math.hpp"

const Math& Math::getInstance()
{
  static Math pInstance;
  return pInstance;
}

Math::Math()
: _identity(
	glm::mat4x4
	{
	  glm::vec4(1, 0, 0, 0),
	  glm::vec4(0, 1, 0, 0),
	  glm::vec4(0, 0, 1, 0),
	  glm::vec4(0, 0, 0, 1)
	}
  )
{ }
