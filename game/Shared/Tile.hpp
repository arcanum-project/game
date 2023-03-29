//
//  Tile.hpp
//  iOS
//
//  Created by Dmitrii Belousov on 9/2/22.
//

#ifndef Tile_hpp
#define Tile_hpp

#include <Metal/Metal.hpp>
#include <vector>
#include <TargetConditionals.h>
#include <unordered_map>
#include <string>

#include "Uniforms.hpp"
#include "Model.hpp"
#include "VertexData.hpp"
#include "GameSettings.h"
#include "MetalConstants.h"
#include "Common/Alignment.hpp"
#include "TileInstanceData.hpp"

class Tile : public Model
{
public:
  Tile(MTL::Device * const pDevice, const uint16_t instanceCount, const uint16_t maxBuffersInFlight);
  ~Tile();
  
  inline const std::unordered_map<uint16_t, TileInstanceData>& getInstanceIdToData() const { return _instanceIdToData; }
  inline const std::vector<VertexData>& getFlippedVertexData() const { return _flippedVertexData; }
  
  inline void update(float_t deltaTime) override
  {
  }

private:
  std::vector<VertexData> _flippedVertexData;
  const uint16_t _instanceCount;
  std::unordered_map<uint16_t, TileInstanceData> _instanceIdToData;
  
  void populateVertexData() override;
  void loadTextures() override;
  const uint16_t makeTexturesFromArt(const char * name, const char * type) const;
};

#endif /* Tile_hpp */
