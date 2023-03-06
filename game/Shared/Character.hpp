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
	static uint32_t frameCounter{_instanceData.walkTexturePixelData.getKeyFrame()};
	static unsigned char currentDirectionIndex{0};
	static uint32_t currentTextureGroupStartIndex{_instanceData.walkTextureStartIndex};
	Uniforms & uf = Uniforms::getInstance();
	const glm::vec3 defaultPosition = Gameplay::getWorldTranslationFromTilePosition(GameplaySettings::CharacterStartRow, GameplaySettings::CharacterStartColumn) * glm::vec4(0.f, 0.f, 0.f, 1.f);
	static PositionWorld newPositionWorld;
	if (position().x == 0.f && position().y == 0.f && position().z == 0.f)
	{
	  setPosition(defaultPosition);
	}
	else
	{
	  if(move(newPositionWorld, deltaTime * GameplaySettings::CharacterMovementSpeed, position(), true))
		setPosition(std::move(newPositionWorld.position));
	}
	uf.setModelMatrix(modelMatrix());
	MTL::Buffer * const pUniformsBuffer = uniformsBuffers().at(frame);
	memcpy(pUniformsBuffer->contents(), & uf, Alignment::roundUpToNextMultipleOf16(sizeof(Uniforms)));
#if defined(TARGET_OSX)
	pUniformsBuffer->didModifyRange(NS::Range(0, pUniformsBuffer->length()));
#endif
	
	const Frame& currentFrame = _instanceData.walkTexturePixelData.frames().at(renderingMetadata.currentTextureIndex - _instanceData.walkTextureStartIndex);
	if (currentDirectionIndex != newPositionWorld.directionIndex)
	{
	  currentDirectionIndex = newPositionWorld.directionIndex;
	  currentTextureGroupStartIndex = _instanceData.walkTextureStartIndex + currentDirectionIndex * _instanceData.walkTexturePixelData.getFrameNum();
	  renderingMetadata.currentTextureIndex = currentTextureGroupStartIndex - 1;
	  frameCounter = _instanceData.walkTexturePixelData.getKeyFrame();
	}
	const bool bShowNextAnimationFrame = frameCounter == _instanceData.walkTexturePixelData.getKeyFrame() ? true : false;
	renderingMetadata.currentTextureIndex = bShowNextAnimationFrame ? renderingMetadata.currentTextureIndex + 1 : renderingMetadata.currentTextureIndex;
	frameCounter = frameCounter == _instanceData.walkTexturePixelData.getKeyFrame() ? 1 : frameCounter + 1;
	if (renderingMetadata.currentTextureIndex - currentTextureGroupStartIndex >= _instanceData.walkTexturePixelData.getFrameNum())
	  renderingMetadata.currentTextureIndex = currentTextureGroupStartIndex;
	renderingMetadata.currentFrameCenterX = RenderingSettings::bApplyTextureCenterOffset ? currentFrame.cx + currentFrame.dx : currentFrame.cx;
	renderingMetadata.currentFrameCenterY = RenderingSettings::bApplyTextureCenterOffset ? currentFrame.cy + currentFrame.dy : currentFrame.cy;
	
	MTL::RenderCommandEncoder * const pRenderEncoder = reinterpret_cast<MTL::RenderCommandEncoder * const>(pCommandEncoder);
	pRenderEncoder->setVertexBuffer(pUniformsBuffer, 0, BufferIndices::UniformsBuffer);
	pRenderEncoder->setVertexBuffer(pVertexBuffer(), 0, BufferIndices::VertexBuffer);
	pRenderEncoder->setVertexBuffer(pIndexBuffer(), 0, BufferIndices::IndexBuffer);
	pRenderEncoder->setFragmentBytes(&renderingMetadata, sizeof(RenderingMetadata), BufferIndices::RenderingMetadataBuffer);
	
	pRenderEncoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, indices().size(), MTL::IndexTypeUInt16, pIndexBuffer(), 0);
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
  };
  
  // Subset of loaded assets data to be passed to GPU to render the current frame
  struct RenderingMetadata
  {
	uint32_t currentTextureIndex;
	uint32_t currentFrameCenterX;
	uint32_t currentFrameCenterY;
  };
  
  CharacterInstanceData _instanceData;
  
  // currentTextureIndex, frame's centerX, frame's centerY
  RenderingMetadata renderingMetadata;
  
  void populateVertexData() override;
  void loadTextures() override;
  void makeTexturesFromArt(const char * name, const char * type);
};

#endif /* Character_hpp */
