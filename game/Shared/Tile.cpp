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
#include "TextureController.hpp"
#include "ObjModelImporter.hpp"
#include "Common/ResourceBundle.hpp"
#include "ArtImporter.hpp"

Tile::Tile(MTL::Device * const pDevice, const uint16_t instanceCount, const uint16_t maxBuffersInFlight)
: Model(pDevice, maxBuffersInFlight),
  _flippedVertexData(),
  _instanceCount(instanceCount),
  _instanceIdToData()
{
	populateVertexData();
	loadTextures();
  }

Tile::~Tile() {
  pIndexBuffer()->release();
  pVertexBuffer()->release();
}

void Tile::populateVertexData() {
  const std::unique_ptr<const ImportedModelData> tile = ObjModelImporter().import("new-tile", "obj");
  setVertexData(tile->vertexData);
  setIndices(tile->indices);
  const std::unique_ptr<const ImportedModelData> tileFlipped = ObjModelImporter().import("new-tile-flipped", "obj");
  _flippedVertexData = tileFlipped->vertexData;
}

/**
 Load terrain textures for crash site sector.
 */
void Tile::loadTextures() {
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ResourceBundle::absolutePath("86570436012", ""), pt);
  const boost::property_tree::ptree tilesArr = pt.get_child("tiles");
  // Iterator to iterate between array items (aka tiles)
  for (boost::property_tree::ptree::const_iterator arrItemsIterator = tilesArr.begin(); arrItemsIterator != tilesArr.end(); ++arrItemsIterator) {
	uint16_t instanceId {};
	std::string textureName {};
	TileInstanceData data {};
	// Iterator to move between fields inside a specific array item (aka tile)
	for (boost::property_tree::ptree::const_iterator arrItemIterator = arrItemsIterator->second.begin(); arrItemIterator != arrItemsIterator->second.end(); ++arrItemIterator) {
	  const std::string key = arrItemIterator->first;
	  if (key.compare("instanceId") == 0) {
		instanceId = boost::lexical_cast<uint16_t>(arrItemIterator->second.data());
	  } else if (key.compare("textureName") == 0) {
		textureName = arrItemIterator->second.data();
		const uint16_t textureIndex = makeTexturesFromArt(("tile/" + textureName).c_str(), "art");
		data.textureIndex = textureIndex;
	  } else if (key.compare("shouldFlip") == 0) {
		bool shouldFlip = boost::lexical_cast<bool>(arrItemIterator->second.data());
		data.shouldFlip = shouldFlip;
		_instanceIdToData.insert(std::make_pair(instanceId, data));
	  } else
		throw std::runtime_error("Unknown tile array format");
	}
  }
}

const uint16_t Tile::makeTexturesFromArt(const char * name, const char * type) const {
  const TextureController& txController = TextureController::instance(pDevice());
  if (txController.textureExist(name, type)) {
	std::cout << "Texture already loaded. Texture name: " << name << std::endl;
	const uint16_t textureIndex = txController.textureIndexByName(name);
	return textureIndex;
  }
  PixelData pd = ArtImporter::importArt(name, type);
  // Tile art only has one frame
  // Tile art only uses first palette
  const std::vector<uint8_t> bgras = pd.bgraFrameFromPalette(0, 0);
  const uint16_t textureIndex = TextureController::instance(pDevice()).loadTexture(name, pd.frames().at(0).imgHeight, pd.frames().at(0).imgWidth, bgras.data());
  return textureIndex;
}
