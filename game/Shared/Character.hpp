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
#include "GameSettings.h"
#include "InputControllerBridge.h"
#include "PixelData.hpp"
#include "TextureController.hpp"
#include "Movement.hpp"
#include "Common/Gameplay.hpp"

class Character : public Model, public Movement {
public:
  Character(MTL::Device * const pDevice);
  ~Character();
  
  inline void render(MTL::CommandEncoder * const pCommandEncoder, const uint16_t & frame, float_t deltaTime) override {
	// Increasing delta time speeds up animation to match the original game
	deltaTime = deltaTime * 2;
	static float_t timeAtCurrentSprite{0.f};
	static unsigned char currentDirectionIndex{8};
	static uint32_t currentTextureGroupStartIndex{_instanceData.walkTextureStartIndex};
	
	unsigned char newDirectionIndex = getDirectionIndex(position());
	if (currentDirectionIndex != newDirectionIndex)
	{
	  currentDirectionIndex = newDirectionIndex;
	  currentTextureGroupStartIndex = _instanceData.walkTextureStartIndex + currentDirectionIndex * _instanceData.walkTexturePixelData.getFrameNum();
	  renderingMetadata.currentTextureIndex = currentTextureGroupStartIndex;
	  timeAtCurrentSprite = deltaTime;
	}
	// For how long we draw the same frame is calculated based on target FPS (60.f)
	const float_t spriteLifetime = (_instanceData.walkTexturePixelData.getKeyFrame() + 1) / 60.f;
	
	const bool bShowNextAnimationFrame = (timeAtCurrentSprite + deltaTime) > spriteLifetime ? true : false;
	renderingMetadata.currentTextureIndex = bShowNextAnimationFrame ? renderingMetadata.currentTextureIndex + 1 : renderingMetadata.currentTextureIndex;
	timeAtCurrentSprite = bShowNextAnimationFrame ? timeAtCurrentSprite + deltaTime - spriteLifetime : timeAtCurrentSprite + deltaTime;
	if (renderingMetadata.currentTextureIndex - currentTextureGroupStartIndex == _instanceData.walkTexturePixelData.getFrameNum())
	  renderingMetadata.currentTextureIndex = currentTextureGroupStartIndex;
	const Frame& newFrame = _instanceData.walkTexturePixelData.frames().at(renderingMetadata.currentTextureIndex - _instanceData.walkTextureStartIndex);
	renderingMetadata.currentFrameCenterX = newFrame.cx;
	renderingMetadata.currentFrameCenterY = newFrame.cy;
	renderingMetadata.currentTextureWidth = newFrame.imgWidth;
	renderingMetadata.currentTextureHeight = newFrame.imgHeight;
	
	Uniforms & uf = Uniforms::getInstance();
	const glm::vec3 defaultPosition = Gameplay::getWorldTranslationFromTilePosition(GameplaySettings::CharacterStartRow, GameplaySettings::CharacterStartColumn) * glm::vec4(0.f, 0.f, 0.f, 1.f);
	if (position().x == 0.f && position().y == 0.f && position().z == 0.f)
	{
	  setPosition(defaultPosition);
	}
	else
	{
	  glm::vec3 outPositionWorld;
	  if (move(outPositionWorld, position(), deltaTime * GameplaySettings::CharacterWalkingSpeed))
	  {
		setPosition(outPositionWorld);
	  }
	}
	uf.setModelMatrix(modelMatrix());
	MTL::Buffer * const pUniformsBuffer = uniformsBuffers().at(frame);
	memcpy(pUniformsBuffer->contents(), & uf, Alignment::roundUpToNextMultipleOf16(sizeof(Uniforms)));
#if defined(TARGET_OSX)
	pUniformsBuffer->didModifyRange(NS::Range(0, pUniformsBuffer->length()));
#endif
	
	MTL::RenderCommandEncoder * const pRenderEncoder = reinterpret_cast<MTL::RenderCommandEncoder * const>(pCommandEncoder);
	pRenderEncoder->setVertexBuffer(pUniformsBuffer, 0, BufferIndices::UniformsBuffer);
	pRenderEncoder->setVertexBytes(&(position()), sizeof(glm::vec3), BufferIndices::VertexBuffer);
	pRenderEncoder->setVertexBytes(&renderingMetadata, sizeof(RenderingMetadata), BufferIndices::RenderingMetadataBuffer);
	pRenderEncoder->setFragmentBytes(&renderingMetadata, sizeof(RenderingMetadata), BufferIndices::RenderingMetadataBuffer);
	
	pRenderEncoder->drawPrimitives(MTL::PrimitiveTypeTriangleStrip, 0, 4, 1);
  }
  
private:
  // Loaded assets data
  struct CharacterInstanceData {
	const char * artName;
	uint8_t frameIndex;
	uint8_t paletteIndex;
	uint16_t standTextureStartIndex;
	PixelData standTexturePixelData;
	uint16_t walkTextureStartIndex;
	PixelData walkTexturePixelData;
	// Coords of quad vertices to put a sprite on
  };
  
  // Subset of loaded assets data to be passed to GPU to render the current frame
  struct RenderingMetadata
  {
	uint32_t currentTextureIndex;
	int32_t currentFrameCenterX;
	int32_t currentFrameCenterY;
	uint32_t currentTextureWidth;
	uint32_t currentTextureHeight;
  };
  
  CharacterInstanceData _instanceData;
  
  // currentTextureIndex, frame's centerX, frame's centerY
  RenderingMetadata renderingMetadata;
  
  void populateVertexData() override;
  void loadTextures() override;
  void makeTexturesFromArt(const char * name, const char * type);
};

#endif /* Character_hpp */
