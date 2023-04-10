//

#include "SpriteRenderPass.h"
#include "Pipelines.hpp"
#include "TextureController.hpp"
#include "MetalConstants.h"
#include "Common/Alignment.hpp"
#include "ArtImporter.hpp"

SpriteRenderPass::SpriteRenderPass(MTL::Device* device, MTL::Library* library, MTL::Buffer* const& materialBuffer)
: device(device),
  pipelineState(nullptr),
  materialBuffer(materialBuffer),
  depthStencilState(nullptr),
  textureData(),
  renderingMetadata()
{
  pipelineState = Pipelines::newPSO(device, library, NS::String::string("spriteVS", NS::UTF8StringEncoding), NS::String::string("spriteFS", NS::UTF8StringEncoding), true);
  buildDepthStencilState();
  
  loadTextures();
}

SpriteRenderPass::~SpriteRenderPass()
{
  pipelineState->release();
  depthStencilState->release();
}

void SpriteRenderPass::buildDepthStencilState()
{
  MTL::DepthStencilDescriptor* depthStencilDesc = MTL::DepthStencilDescriptor::alloc()->init();
  depthStencilDesc->setDepthCompareFunction(MTL::CompareFunctionLess);
  depthStencilDesc->setDepthWriteEnabled(true);
  depthStencilState = device->newDepthStencilState(depthStencilDesc);
  depthStencilDesc->release();
}

void SpriteRenderPass::makeTexturesFromArt(const char* name, const char* type, PixelData* const pixelDataOut, uint16_t& textureStartIndexOut)
{
  ArtImporter::importArt(pixelDataOut, name, "art");
  const uint8_t defaultPaletteIndex = 2;
  bool isTextureIndexSet = false;
  for (ushort i = 0; i < pixelDataOut->frames().size(); ++i)
  {
	const std::vector<uint8_t> bgras = pixelDataOut->bgraFrameFromPalette(i, defaultPaletteIndex);
	TextureController& txController = TextureController::instance(device);
	const uint16_t txIndex = txController.loadTexture(name, pixelDataOut->frames().at(i).imgHeight, pixelDataOut->frames().at(i).imgWidth, bgras.data());
	textureData.artName = name;
	textureData.frameIndex = i;
	textureData.paletteIndex = defaultPaletteIndex;
	textureStartIndexOut = isTextureIndexSet ? textureStartIndexOut : txIndex;
	isTextureIndexSet = true;
  }
  renderingMetadata.currentTextureIndex = textureData.walkTextureStartIndex;
}

void SpriteRenderPass::loadTextures()
{
  makeTexturesFromArt("hmfc2xab", "art", textureData.walkTexturePixelData, textureData.walkTextureStartIndex);
}


void SpriteRenderPass::draw(MTL::CommandBuffer* commandBuffer, CA::MetalDrawable* drawable, MTL::Texture* depthTexture, GameScene* scene, float_t deltaTime)
{
  MTL::RenderPassDescriptor* rpd = MTL::RenderPassDescriptor::alloc()->init();
  rpd->colorAttachments()->object(0)->setTexture(drawable->texture());
  rpd->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionLoad);

  MTL::RenderPassDepthAttachmentDescriptor* depthAttachmentDescriptor = MTL::RenderPassDepthAttachmentDescriptor::alloc()->init();
  depthAttachmentDescriptor->setTexture(depthTexture);
  rpd->setDepthAttachment(depthAttachmentDescriptor);
  depthAttachmentDescriptor->release();

  MTL::RenderCommandEncoder* renderEncoder = commandBuffer->renderCommandEncoder(rpd);
  rpd->release();
  renderEncoder->setLabel(NS::String::string("Sprite Render Encoder", NS::UTF8StringEncoding));
  renderEncoder->setRenderPipelineState(pipelineState);
  renderEncoder->setDepthStencilState(depthStencilState);
  renderEncoder->setFragmentBuffer(materialBuffer, 0, BufferIndices::TextureBuffer);
  renderEncoder->useHeap(TextureController::instance(device).heap());
  for (Sprite* sprite : scene->getSprites())
  {
	sprite->update(deltaTime);
	
	updateSpriteTexture(deltaTime, sprite);
	
	Uniforms& uf = Uniforms::getInstance();
	uf.setModelMatrix(sprite->modelMatrix());
	
	renderEncoder->setVertexBytes(&uf, Alignment::roundUpToNextMultipleOf16(sizeof(Uniforms)), BufferIndices::UniformsBuffer);
	renderEncoder->setVertexBytes(&(sprite->position()), sizeof(glm::vec3), BufferIndices::VertexBuffer);
	renderEncoder->setVertexBytes(&renderingMetadata, sizeof(RenderingMetadata), BufferIndices::RenderingMetadataBuffer);
	renderEncoder->setFragmentBytes(&renderingMetadata, sizeof(RenderingMetadata), BufferIndices::RenderingMetadataBuffer);
	
	renderEncoder->drawPrimitives(MTL::PrimitiveTypeTriangleStrip, 0, 4, 1);
  }
  renderEncoder->endEncoding();
}

void SpriteRenderPass::updateSpriteTexture(float_t deltaTime, Sprite* sprite)
{
  // Increasing delta time speeds up animation to match the original game
  deltaTime *= 2;
  sprite->setTimeAtCurrentTexture(sprite->getTimeAtCurrentTexture() + deltaTime);
  static uint32_t currentTextureGroupStartIndex{textureData.walkTextureStartIndex};
  
  const uint8_t newDirectionIndex = sprite->getDirectionIndex(sprite->position());
  if (sprite->getCurrentDirectionIndex() != newDirectionIndex)
  {
	sprite->setCurrentDirectionIndex(newDirectionIndex);
	currentTextureGroupStartIndex = textureData.walkTextureStartIndex + sprite->getCurrentDirectionIndex() * textureData.walkTexturePixelData->getFrameNum();
	renderingMetadata.currentTextureIndex = currentTextureGroupStartIndex;
	sprite->setTimeAtCurrentTexture(0.f);
  }
  else
  {
	// For how long we draw the same frame is calculated based on target FPS (60)
	const float_t spriteLifetime = ((float_t) (textureData.walkTexturePixelData->getKeyFrame() + 1)) / 60;
	
	const bool bShowNextAnimationFrame = sprite->getTimeAtCurrentTexture() > spriteLifetime ? true : false;
	if (bShowNextAnimationFrame)
	{
	  renderingMetadata.currentTextureIndex = renderingMetadata.currentTextureIndex + 1;
	  sprite->setTimeAtCurrentTexture(sprite->getTimeAtCurrentTexture() - spriteLifetime);
	}
	
	if (renderingMetadata.currentTextureIndex - currentTextureGroupStartIndex == textureData.walkTexturePixelData->getFrameNum()) renderingMetadata.currentTextureIndex = currentTextureGroupStartIndex;
  }
  
  const Frame& newFrame = textureData.walkTexturePixelData->frames().at(renderingMetadata.currentTextureIndex - textureData.walkTextureStartIndex);
  renderingMetadata.currentFrameCenterX = newFrame.cx;
  renderingMetadata.currentFrameCenterY = newFrame.cy;
  renderingMetadata.currentTextureWidth = newFrame.imgWidth;
  renderingMetadata.currentTextureHeight = newFrame.imgHeight;
}
