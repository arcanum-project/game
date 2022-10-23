//
//  Character.hpp
//  iOS
//
//  Created by Dmitrii Belousov on 9/3/22.
//

#ifndef Character_hpp
#define Character_hpp

#include <Metal/Metal.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include "Uniforms.hpp"
#include "Model.hpp"
#include "Common/Alignment.hpp"
#include "Constants.h"
#include "InputControllerBridge.h"

class Character : public Model {
public:
  Character(MTL::Device * const pDevice);
  ~Character();
  
  inline void render(MTL::CommandEncoder * const pCommandEncoder, const uint16_t & frame) override {
	Uniforms & uf = Uniforms::getInstance();
	// Get touch coordinates in NDC
	const float xCoord = xCoordinate();
	const float yCoord = yCoordinate();
	glm::vec3 xyNDC = glm::vec3();
	if (xCoord != 0.0f || yCoord != 0.0f) {
	  // Move character to where the player clicked / touched the screen
	  // Based on: https://metalbyexample.com/picking-hit-testing/
	  xyNDC = Math::getInstance().screenToNDC(xCoord, yCoord, uf.drawableWidth(), uf.drawableHeight());
	  const glm::mat4x4 inverseProjectionMatrix = glm::inverse(uf.getProjectionMatrix());
	  const glm::mat4x4 inverseViewMatrix = glm::inverse(uf.getViewMatrix());
	  const glm::vec4 direction = inverseViewMatrix * inverseProjectionMatrix * glm::vec4(xyNDC, 1.0f);
	  setPosition(glm::vec3(direction.x / direction.w, direction.y / direction.w, direction.z / direction.w));
	}
	
	uf.setModelMatrix(modelMatrix());
	MTL::Buffer * const pUniformsBuffer = uniformsBuffers().at(frame);
	memcpy(pUniformsBuffer->contents(), & uf, Alignment::roundUpToNextMultipleOf16(sizeof(Uniforms)));
#if defined(TARGET_OSX)
	pUniformsBuffer->didModifyRange(NS::Range(0, pUniformsBuffer->length()));
#endif
	
	MTL::RenderCommandEncoder * const pRenderEncoder = reinterpret_cast<MTL::RenderCommandEncoder * const>(pCommandEncoder);
	pRenderEncoder->setVertexBuffer(pUniformsBuffer, 0, BufferIndices::UniformsBuffer);
	pRenderEncoder->setVertexBuffer(pVertexBuffer(), 0, BufferIndices::VertexBuffer);
	pRenderEncoder->setVertexBuffer(pIndexBuffer(), 0, BufferIndices::IndexBuffer);
	pRenderEncoder->setVertexBytes(& xyNDC, Alignment::roundUpToNextMultipleOf16(sizeof(glm::vec3)), 23);
	pRenderEncoder->setFragmentBytes(& _textureIndex, sizeof(uint16_t), 19);
	
	pRenderEncoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, indices().size(), MTL::IndexTypeUInt16, pIndexBuffer(), 0);
  }
  
private:
  uint16_t _textureIndex;
  
  void populateVertexData() override;
  void loadTextures() override;
  
protected:
  const TextureData makeTexturesFromArt(const char * name, const char * type) const override;
};

#endif /* Character_hpp */
