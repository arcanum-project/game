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
#include <utility>

#include "Uniforms.hpp"
#include "Model.hpp"
#include "Common/Alignment.hpp"
#include "MetalConstants.h"
#include "GameSettings.hpp"
#include "InputControllerBridge.h"
#include "PixelData.hpp"
#include "TextureController.hpp"
#include "Movement.hpp"
#include "Common/Gameplay.hpp"

class Character : public Model, public Movement {
public:
  Character(MTL::Device * const pDevice);
  ~Character();
  
  inline void render(MTL::CommandEncoder * const pCommandEncoder, const uint16_t & frame, const float_t deltaTime) override {
	Uniforms & uf = Uniforms::getInstance();
	_instanceData.currentTextureIndex = _instanceData.baseTextureIndex;
	const glm::vec3 defaultPosition = Gameplay::getWorldTranslationFromTilePosition(GameplaySettings::CharacterStartRow, GameplaySettings::CharacterStartColumn) * glm::vec4(0.f, 0.f, 0.f, 1.f);
	if (position().x == 0.f && position().y == 0.f && position().z == 0.f)
	{
	  setPosition(defaultPosition);
	}
	else
	{
	  glm::vec3 newPositionWorld{};
	  if(move(newPositionWorld, deltaTime * GameplaySettings::CharacterMovementSpeed, position()))
		setPosition(std::move(newPositionWorld));
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
	pRenderEncoder->setFragmentBytes(& _instanceData.currentTextureIndex, sizeof(uint16_t), 19);
	
	pRenderEncoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, indices().size(), MTL::IndexTypeUInt16, pIndexBuffer(), 0);
  }
  
private:
  struct CharacterInstanceData {
	const char * artName;
	uint8_t frameIndex;
	uint8_t paletteIndex;
	uint16_t baseTextureIndex;
	uint16_t currentTextureIndex;
	PixelData pixelData;
  };
  
  CharacterInstanceData _instanceData;
  
  void populateVertexData() override;
  void loadTextures() override;
  void makeTexturesFromArt(const char * name, const char * type);
};

#endif /* Character_hpp */
