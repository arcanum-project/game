//
//  ObjImporter.cpp
//  fragments
//
//  Created by Dmitrii Belousov on 7/10/22.
//

#include "ObjModelImporter.hpp"
#include <vector>
#include <array>
#include <iostream>
#include <fstream>

Model * const ObjModelImporter::import(const char * resourceName, const char * resourceType) const
{
  std::string line;
  std::ifstream file;
  try {
	file.open(absolutePath(resourceName, resourceType), std::ifstream::in);
	if (file.is_open())
	{
	  while (std::getline(file, line))
	  {
		std::cout << line << std::endl;
	  }
	}
	file.close();
  } catch (std::system_error & e) {
	if (file.is_open())
	  file.close();
	std::cerr << e.code().message() << std::endl;
	throw;
  }
  
  return new Model(std::vector<std::array<glm::vec3, 2>>(), std::vector<uint16_t>(), nullptr);
}
