//
//  ObjImporter.cpp
//  fragments
//
//  Created by Dmitrii Belousov on 7/10/22.
//

#include "ObjModelImporter.hpp"
#include <iostream>
#include <fstream>

Model * const ObjModelImporter::import(void * const pDevice, const char * resourceName, const char * resourceType) const {
  std::vector<VertexData> vertexData {};
  std::vector<uint16_t> indices {};
  
  std::string line;
  std::ifstream file;
  try {
	file.open(absolutePath(resourceName, resourceType), std::ifstream::in);
	if (file.is_open()) {
	  std::vector<glm::vec3> vertices {};
	  std::vector<glm::vec2> textures {};
	  std::vector<glm::vec3> normals {};
	  uint16_t currentIndex = 0;
	  
	  while (std::getline(file, line)) {
		std::string delimiter = " ";
		size_t pos = 0;
		std::string token {};
		
		if ((pos = line.find(delimiter)) != std::string::npos)
		  token = token = line.substr(0, pos);
		
		if (token.length() > 0) {
		  // Substring to the actual data
		  line.erase(0, pos + delimiter.length());
		  
		  bool isVertex = token.compare("v") == 0 ? true : false;
		  bool isTexture = token.compare("vt") == 0 ? true : false;
		  bool isResized = false;
		  if (isVertex || isTexture || token.compare("vn") == 0) {
			processNonFace(line, vertices, textures, normals, isTexture, isVertex);
		  } else if (token.compare("f") == 0) {
			if (!isResized) {
			  // Allocate space for both vertexData and indices to be just enough to fit number of vertices in a model
			  // This assumes that faces begin processing after all vertices have been processed
			  vertexData.resize(vertices.size());
			  isResized = true;
			}
			processFace(line, vertexData, indices, currentIndex, vertices, textures, normals);
		  } else {
			std::cout << "Unexpected data type in .obj file: " + token << std::endl;
		  }
		}
	  }
	}
	file.close();
  } catch (std::system_error & e) {
	if (file.is_open())
	  file.close();
	std::cerr << e.code().message() << std::endl;
	throw;
  }
  
  return new Model(vertexData, indices, pDevice);
}
