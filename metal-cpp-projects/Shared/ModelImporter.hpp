//
//  Modellmporter.hpp
//  metal-cpp-projects
//
//  Created by Dmitrii Belousov on 7/10/22.
//

#pragma once

#include "Model.hpp"

class ModelImporter
{
public:
  virtual ~ModelImporter(){};
  virtual Model * const import(const char * resourceName, const char * resourceType) const = 0;
};
