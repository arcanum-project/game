//

#include "SpriteRenderPass.h"
#include "Pipelines.hpp"
#include "TextureController.hpp"

SpriteRenderPass::SpriteRenderPass(MTL::Device* device, MTL::Library* library, MTL::Buffer* textureBuffer)
: device(device),
  pipelineState(nullptr),
  textureBuffer(textureBuffer),
  depthStencilState(nullptr)
{
  pipelineState = Pipelines::newPSO(device, library, NS::String::string("spriteVS", NS::UTF8StringEncoding), NS::String::string("spriteFS", NS::UTF8StringEncoding), true);
  buildDepthStencilState();
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

void SpriteRenderPass::draw(MTL::CommandBuffer* commandBuffer, CA::MetalDrawable* drawable, MTL::Texture* depthTexture, GameScene* scene, float_t deltaTime)
{
  MTL::RenderPassDescriptor* rpd = MTL::RenderPassDescriptor::alloc()->init();
  rpd->colorAttachments()->object(0)->setTexture(drawable->texture());
  rpd->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionLoad);

  MTL::RenderPassDepthAttachmentDescriptor* depthAttachmentDescriptor = MTL::RenderPassDepthAttachmentDescriptor::alloc()->init();
  depthAttachmentDescriptor->setTexture(depthTexture);
  rpd->setDepthAttachment(depthAttachmentDescriptor);

  MTL::RenderCommandEncoder* renderEncoder = commandBuffer->renderCommandEncoder(rpd);
  renderEncoder->setLabel(NS::String::string("Sprite Render Encoder", NS::UTF8StringEncoding));
  renderEncoder->setRenderPipelineState(pipelineState);
  renderEncoder->setDepthStencilState(depthStencilState);
  renderEncoder->setFragmentBuffer(textureBuffer, 0, BufferIndices::TextureBuffer);
  renderEncoder->useHeap(TextureController::instance(device).heap());
  for (Model* sprite : scene->getSprites())
	sprite->render(renderEncoder, 0, deltaTime);
  renderEncoder->endEncoding();

  depthAttachmentDescriptor->release();
  rpd->release();
}
