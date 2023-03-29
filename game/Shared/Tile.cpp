//
//  Tile.cpp
//  iOS
//
//  Created by Dmitrii Belousov on 9/2/22.
//

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <string_view>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>

#include "Tile.hpp"
#include "ObjModelImporter.hpp"

Tile::Tile(const uint16_t instanceCount, const uint16_t maxBuffersInFlight)
: vertexData(),
  indices(),
  flippedVertexData(),
  instanceCount(instanceCount)
{
  populateVertexData();
}

void Tile::populateVertexData() {
  const std::unique_ptr<const ImportedModelData> tile = ObjModelImporter().import("new-tile", "obj");
  vertexData = std::move(tile->vertexData);
  indices = std::move(tile->indices);
  const std::unique_ptr<const ImportedModelData> tileFlipped = ObjModelImporter().import("new-tile-flipped", "obj");
  flippedVertexData = std::move(tileFlipped->vertexData);
}
