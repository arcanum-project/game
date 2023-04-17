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

Renderer::Renderer(SDL_Window* window)
: window(window),
  sdlRenderer(createSDLRenderer()),
  layer((CA::MetalLayer*)SDL_RenderGetMetalLayer(sdlRenderer)),
  device(layer->device()),
  commandQueue(device->newCommandQueue()),
  library(device->newDefaultLibrary()),
  depthTexture(nullptr),
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
  
  int drawableWidth, drawableHeight;
  SDL_Metal_GetDrawableSize(window, &drawableWidth, &drawableHeight);
  createDepthTexture(drawableWidth, drawableHeight);
  // Necessary to initialize uniforms
  drawableSizeWillChange(drawableWidth, drawableHeight);
  
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
  depthTexture->release();
  SDL_DestroyRenderer(sdlRenderer);
}

SDL_Renderer* Renderer::createSDLRenderer()
{
  SDL_SetHint(SDL_HINT_RENDER_DRIVER, "metal");
  return SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
}

void Renderer::createDepthTexture(float_t drawableWidth, float_t drawableHeight)
{
  MTL::TextureDescriptor* depthTextureDescriptor = MTL::TextureDescriptor::texture2DDescriptor(MTL::PixelFormatDepth32Float, drawableWidth, drawableHeight, false);
  depthTextureDescriptor->setStorageMode(MTL::StorageModePrivate);
  depthTextureDescriptor->setUsage(MTL::TextureUsageRenderTarget);
  depthTexture = device->newTexture(depthTextureDescriptor);
  depthTextureDescriptor->release();
  depthTexture->setLabel(NS::String::string("DepthTexture", NS::UTF8StringEncoding));
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

void Renderer::drawFrame()
{
  NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();
  
  // We are reusing same command buffers for sending commands to GPU. Therefore we must lock to ensure that buffers are only used when GPU is done with them.
  // Check this article for more: https://crimild.wordpress.com/2016/05/19/praise-the-metal-part-1-rendering-a-single-frame/
  dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
  
  MTL::CommandBuffer* commandBuffer = commandQueue->commandBuffer();
  commandBuffer->addCompletedHandler(^void(MTL::CommandBuffer* commandBuffer) {
	dispatch_semaphore_signal(this->semaphore);
  });
  
  const std::chrono::time_point<std::chrono::system_clock> currentTimeSeconds = std::chrono::system_clock::now();
  const std::chrono::duration<float_t> deltaTime = currentTimeSeconds - lastTimeSeconds;
  lastTimeSeconds = currentTimeSeconds;
  // Updating scene before drawing follows Metal performance best practice: to delay acquisition of next drawable for as long as possible
  gameScene->update(deltaTime.count());
  
  Uniforms& uf = Uniforms::getInstance();
  uf.setViewMatrix(gameScene->pCamera()->viewMatrix());
  uf.setProjectionMatrix(gameScene->pCamera()->projectionMatrix());
  
  CA::MetalDrawable* nextDrawable = layer->nextDrawable();
  
  frame = (frame + 1) % RenderingSettings::MaxBuffersInFlight;
  tileRenderPass->draw(commandBuffer, nextDrawable, depthTexture, gameScene, deltaTime.count(), frame);
  spriteRenderPass->draw(commandBuffer, nextDrawable, depthTexture, gameScene, deltaTime.count());
  
  commandBuffer->presentDrawable(nextDrawable);
  commandBuffer->commit();
  
  pool->release();
  
  // Reset touch / click coordinates
  setCoordinates(.0f, .0f);
}

void Renderer::drawableSizeWillChange(const float_t drawableWidth, const float_t drawableHeight) {
  Uniforms& uf = Uniforms::getInstance();
  uf.setDrawableWidth(drawableWidth);
  uf.setDrawableHeight(drawableHeight);
  gameScene->update(uf.drawableWidth(), uf.drawableHeight());
}
