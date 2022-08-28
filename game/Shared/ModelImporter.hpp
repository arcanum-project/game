//
//  Modellmporter.hpp
//  metal-cpp-projects
//
//  Created by Dmitrii Belousov on 7/10/22.
//

#pragma once

#include <stdio.h>
#include <vector>
#include <memory>

#include "VertexData.hpp"

struct ImportedModelData {
  ImportedModelData(const std::vector<VertexData> vData, const std::vector<uint16_t> i)
  : vertexData(vData),
	indices(i)
  {};
  
  const std::vector<VertexData> vertexData;
  const std::vector<uint16_t> indices;
  
};

class ModelImporter
{
public:
  virtual ~ModelImporter(){};
  virtual const std::unique_ptr<const ImportedModelData> import(const char * resourceName, const char * resourceType) const = 0;
};
