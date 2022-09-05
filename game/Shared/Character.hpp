//
//  Character.hpp
//  iOS
//
//  Created by Dmitrii Belousov on 9/3/22.
//

#ifndef Character_hpp
#define Character_hpp

#include <Metal/Metal.hpp>

#include "Uniforms.hpp"
#include "Model.hpp"
#include "Common/Alignment.hpp"
#include "Constants.h"

class Character : public Model {
public:
  Character(MTL::Device * const pDevice);
  ~Character();
  
  inline void render(MTL::CommandEncoder * const pCommandEncoder, const uint16_t & frame) override {
	Uniforms & uf = Uniforms::getInstance();
	// Elevate model to avoid flickering due to collision with tiles
	setPosition(glm::vec3(4.0f, .0f, 4.0f));
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
	pRenderEncoder->setFragmentBytes(& _textureIndex, sizeof(uint16_t), 19);
	
	pRenderEncoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, indices().size(), MTL::IndexTypeUInt16, pIndexBuffer(), 0);
  }
  
private:
  uint16_t _textureIndex;
  
  void populateVertexData() override;
  void loadTextures() override;
};

#endif /* Character_hpp */
