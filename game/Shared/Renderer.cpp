//
//  Renderer.cpp
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 7/24/22.
//

#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION

#include "Renderer.hpp"
#include "MetalConstants.h"
#include "TextureController.hpp"
#include "Pipelines.hpp"

Renderer::Renderer(MTL::Device* device)
: device(device->retain()),
  commandQueue(device->newCommandQueue()),
  library(device->newDefaultLibrary()),
  materialBuffer(nullptr),
  gameScene(new GameScene()),
  frame(0),
  semaphore(dispatch_semaphore_create(RenderingSettings::MaxBuffersInFlight)),
  lastTimeSeconds(std::chrono::system_clock::now()),
  tileRenderPass(nullptr),
  spriteRenderPass(nullptr)
{
	// Initialize touch / click coordinates
	setCoordinates(.0f, .0f);
  
  tileRenderPass = new TileRenderPass(this->device, library, materialBuffer, RenderingSettings::NumOfTilesPerSector, RenderingSettings::MaxBuffersInFlight, gameScene);
  spriteRenderPass = new SpriteRenderPass(this->device, library, materialBuffer);
  
  initializeTextures();
}

Renderer::~Renderer() {
  delete tileRenderPass;
  delete spriteRenderPass;
  delete gameScene;
  materialBuffer->release();
  library->release();
  commandQueue->release();
  device->release();
}

void Renderer::initializeTextures() {
  TextureController& txController = TextureController::instance(device);
  txController.makeHeap();
  txController.moveTexturesToHeap(commandQueue);
  
  NS::Error* error = nullptr;
  MTL::FunctionConstantValues* fnConstantValues = MTL::FunctionConstantValues::alloc()->init();
  MTL::Function* spriteFragmentFn = library->newFunction(NS::String::string("spriteFS", NS::UTF8StringEncoding), fnConstantValues, &error);
  fnConstantValues->release();
  if (!spriteFragmentFn)
	__builtin_printf("Error creating fragment function. Error: %s", error->localizedDescription()->utf8String());
  error->release();
  
  MTL::ArgumentEncoder* argumentEncoder = spriteFragmentFn->newArgumentEncoder(BufferIndices::TextureBuffer);
  materialBuffer = device->newBuffer(argumentEncoder->encodedLength(), MTL::ResourceStorageModeShared);
  argumentEncoder->setArgumentBuffer(materialBuffer, 0);
  argumentEncoder->setTextures(txController.textures().data(), NS::Range(0, txController.textures().size()));
  argumentEncoder->setTexture(txController.textures().at(0), 0);
  
  argumentEncoder->release();
}

void Renderer::drawFrame(CA::MetalDrawable* drawable, MTL::Texture* depthTexture) {
  // We are reusing same buffers for passing tile instances data to GPU. Therefore we must lock to ensure that buffers are only used when GPU is done with them.
  // Check this article for more: https://crimild.wordpress.com/2016/05/19/praise-the-metal-part-1-rendering-a-single-frame/
  dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
  
  MTL::CommandBuffer* commandBuffer = commandQueue->commandBuffer();
  commandBuffer->addCompletedHandler(^void(MTL::CommandBuffer* commandBuffer) {
	dispatch_semaphore_signal(this->semaphore);
  });
  
  const std::chrono::time_point<std::chrono::system_clock> currentTimeSeconds = std::chrono::system_clock::now();
  const std::chrono::duration<float_t> deltaTime = currentTimeSeconds - lastTimeSeconds;
  lastTimeSeconds = currentTimeSeconds;
  gameScene->update(deltaTime.count());
  
  Uniforms& uf = Uniforms::getInstance();
  uf.setViewMatrix(gameScene->pCamera()->viewMatrix());
  uf.setProjectionMatrix(gameScene->pCamera()->projectionMatrix());
  frame = (frame + 1) % RenderingSettings::MaxBuffersInFlight;
  
  tileRenderPass->draw(commandBuffer, drawable, depthTexture, gameScene, deltaTime.count(), frame);
  spriteRenderPass->draw(commandBuffer, drawable, depthTexture, gameScene, deltaTime.count());
  
  commandBuffer->presentDrawable(drawable);
  commandBuffer->commit();
  
  // Reset touch / click coordinates
  setCoordinates(.0f, .0f);
}

void Renderer::drawableSizeWillChange(const float_t drawableWidth, const float_t drawableHeight) {
  Uniforms& uf = Uniforms::getInstance();
  uf.setDrawableWidth(drawableWidth);
  uf.setDrawableHeight(drawableHeight);
  gameScene->update(uf.drawableWidth(), uf.drawableHeight());
}
