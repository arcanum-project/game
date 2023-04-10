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

void SpriteRenderPass::makeTexturesFromArt(const char* name, const char* type)
{
  PixelData pd = ArtImporter::importArt(name, "art");
  const uint8_t defaultPaletteIndex = 2;
  bool isTextureIndexSet {false};
  for (ushort i = 0; i < pd.frames().size(); ++i) {
	const std::vector<uint8_t> bgras = pd.bgraFrameFromPalette(i, defaultPaletteIndex);
	// Can pass nullptr because at this moment TextureController has already been initialized
	// Using nullptr as a hack to avoid keeping a reference to MTL::Device
	TextureController& txController = TextureController::instance(device);
	const uint16_t txIndex = txController.loadTexture(name, pd.frames().at(i).imgHeight, pd.frames().at(i).imgWidth, bgras.data());
	textureData.artName = name;
	textureData.frameIndex = i;
	textureData.paletteIndex = defaultPaletteIndex;
	// We do this to ensure that we have an index of the first frame.
	// Since frames are stored contiguously, with start frame and offset we can get any frame we need.
	if (!isTextureIndexSet) {
	  // Get pointer to last char in name - it will define what type of animation this texture is for
	  while (*name++ != '\0')
		;
	  // Have to do it, because after while loop name points at the next char after '\0'
	  name -= 2;
	  switch (*name) {
		case 'a':
		  textureData.standTextureStartIndex = txIndex;
		  textureData.standTexturePixelData = pd;
		  isTextureIndexSet = true;
		  break;
		case 'b':
		  textureData.walkTextureStartIndex = txIndex;
		  textureData.walkTexturePixelData = pd;
		  isTextureIndexSet = true;
		  break;
		default:
		  break;
	  }
	}
  }
  renderingMetadata.currentTextureIndex = textureData.walkTextureStartIndex;
}

void SpriteRenderPass::loadTextures()
{
  makeTexturesFromArt("hmfc2xab", "art");
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
  static uint32_t currentTextureGroupStartIndex{textureData.walkTextureStartIndex};
  
  const uint8_t newDirectionIndex = sprite->getDirectionIndex(sprite->position());
  if (sprite->getCurrentDirectionIndex() != newDirectionIndex)
  {
	sprite->setCurrentDirectionIndex(newDirectionIndex);
	currentTextureGroupStartIndex = textureData.walkTextureStartIndex + sprite->getCurrentDirectionIndex() * textureData.walkTexturePixelData.getFrameNum();
	renderingMetadata.currentTextureIndex = currentTextureGroupStartIndex;
	sprite->setTimeAtCurrentTexture(deltaTime);
  }
  // For how long we draw the same frame is calculated based on target FPS (60.f)
  const float_t spriteLifetime = (textureData.walkTexturePixelData.getKeyFrame() + 1) / 60.f;
  
  const bool bShowNextAnimationFrame = (sprite->getTimeAtCurrentTexture() + deltaTime) > spriteLifetime ? true : false;
  renderingMetadata.currentTextureIndex = bShowNextAnimationFrame ? renderingMetadata.currentTextureIndex + 1 : renderingMetadata.currentTextureIndex;
  
  if (bShowNextAnimationFrame) sprite->setTimeAtCurrentTexture(sprite->getTimeAtCurrentTexture() + deltaTime - spriteLifetime);
  else sprite->setTimeAtCurrentTexture(sprite->getTimeAtCurrentTexture() + deltaTime);
  
  if (renderingMetadata.currentTextureIndex - currentTextureGroupStartIndex == textureData.walkTexturePixelData.getFrameNum()) renderingMetadata.currentTextureIndex = currentTextureGroupStartIndex;
  
  const Frame& newFrame = textureData.walkTexturePixelData.frames().at(renderingMetadata.currentTextureIndex - textureData.walkTextureStartIndex);
  renderingMetadata.currentFrameCenterX = newFrame.cx;
  renderingMetadata.currentFrameCenterY = newFrame.cy;
  renderingMetadata.currentTextureWidth = newFrame.imgWidth;
  renderingMetadata.currentTextureHeight = newFrame.imgHeight;
}
