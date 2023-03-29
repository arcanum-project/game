//  Created by Dmitrii Belousov on 9/2/22.
//

#pragma once

#include <vector>
#include <TargetConditionals.h>
#include <unordered_map>

#include "VertexData.hpp"
#include "TileInstanceData.hpp"
#include "Transformable.hpp"

class Tile : public Transformable
{
public:
  Tile(const uint16_t instanceCount, const uint16_t maxBuffersInFlight);
  ~Tile() = default;
  
  inline const std::vector<VertexData>& getVertexData() const { return vertexData; }
  inline const std::vector<uint16_t>& getIndices() const { return indices; }
  inline const std::vector<VertexData>& getFlippedVertexData() const { return flippedVertexData; }
  
  inline void update(float_t deltaTime)
  {
  }

private:
  std::vector<VertexData> vertexData;
  std::vector<uint16_t> indices;
  std::vector<VertexData> flippedVertexData;
  const uint16_t instanceCount;
  
  void populateVertexData();
};
