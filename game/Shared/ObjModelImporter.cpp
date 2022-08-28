//
//  ObjImporter.cpp
//  fragments
//
//  Created by Dmitrii Belousov on 7/10/22.
//

#include <iostream>
#include <fstream>
#include <stdio.h>

#include "ObjModelImporter.hpp"
#include "Common/ResourceBundle.hpp"

const std::unique_ptr<const ImportedModelData> ObjModelImporter::import(const char * resourceName, const char * resourceType) const {
  std::vector<VertexData> vertexData {};
  std::vector<uint16_t> indices {};
  std::unordered_map<std::string, uint16_t> vertexToIndex {};

  std::string line;
  std::ifstream file;
  try {
	file.open(ResourceBundle::absolutePath(resourceName, resourceType), std::ifstream::in);
	if (file.is_open()) {
	  std::vector<glm::vec3> vertices {};
	  std::vector<glm::vec2> textures {};
	  std::vector<glm::vec3> normals {};
	  
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
		  if (isVertex || isTexture || token.compare("vn") == 0) {
			processNonFace(line, vertices, textures, normals, isTexture, isVertex);
		  } else if (token.compare("f") == 0) {
			processFace(line, vertexData, indices, vertexToIndex, vertices, textures, normals);
		  } else {
			std::cout << "Skipping unknown line classifier in .obj file: " + token << std::endl;
		  }
		}
	  }
	  file.close();
	}
  } catch (std::system_error & e) {
	if (file.is_open())
	  file.close();
	std::cerr << e.code().message() << std::endl;
	throw;
  }

  return std::make_unique<const ImportedModelData>(vertexData, indices);
}
