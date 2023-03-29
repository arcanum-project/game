//

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <string_view>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>

#include "TileRenderPass.h"
#include "Pipelines.hpp"
#include "Common/Alignment.hpp"
#include "MetalConstants.h"
#include "TextureController.hpp"
#include "Common/ResourceBundle.hpp"
#include "ArtImporter.hpp"

TileRenderPass::TileRenderPass(MTL::Device* device, MTL::Library* library, MTL::Buffer* const& materialBuffer, const uint16_t instanceCount, const uint16_t maxBuffersInFlight, GameScene* scene)
: device(device),
  renderPipelineState(nullptr),
  materialBuffer(materialBuffer),
  depthStencilState(nullptr),
  indirectCommandBuffer(nullptr),
  icbArgumentBuffer(nullptr),
  tileVisibilityKernelFn(nullptr),
  computePipelineState(nullptr),
  instanceDataBuffers(std::vector<MTL::Buffer*>(maxBuffersInFlight)),
  flippedVertexBuffer(nullptr),
  vertexBuffer(nullptr),
  indexBuffer(nullptr),
  instanceIdToData(),
  uniformsBuffers(std::vector<MTL::Buffer*>(maxBuffersInFlight))
{
  loadTextures(scene);
  buildPipelineStates(library);
  buildDepthStencilState();
  buildIndirectCommandBuffer();
  
  const size_t instanceDataSize = instanceCount * Alignment::roundUpToNextMultipleOf16(sizeof(TileInstanceData));
  for (size_t i = 0; i < maxBuffersInFlight; i++) {
	instanceDataBuffers[i] = device->newBuffer(instanceDataSize, MTL::ResourceStorageModeShared);
	NS::String* label = NS::String::string("InstanceData ", NS::UTF8StringEncoding)->stringByAppendingString(NS::String::string(std::to_string(i).c_str(), NS::UTF8StringEncoding));
	instanceDataBuffers[i]->setLabel(label);
  }
  
  const size_t uniformsSize = Alignment::roundUpToNextMultipleOf16(sizeof(Uniforms));
  for (size_t i = 0; i < maxBuffersInFlight; i++) {
	uniformsBuffers[i] = device->newBuffer(uniformsSize, MTL::ResourceStorageModeShared);
	NS::String* label = NS::String::string("Uniforms ", NS::UTF8StringEncoding)->stringByAppendingString(NS::String::string(std::to_string(i).c_str(), NS::UTF8StringEncoding));
	uniformsBuffers[i]->setLabel(label);
  }
	
  buildVertexBuffers(scene);
}

TileRenderPass::~TileRenderPass()
{
  computePipelineState->release();
  tileVisibilityKernelFn->release();
  icbArgumentBuffer->release();
  indirectCommandBuffer->release();
  renderPipelineState->release();
  depthStencilState->release();
  for (MTL::Buffer* buffer : instanceDataBuffers) {
	buffer->release();
  }
  flippedVertexBuffer->release();
  vertexBuffer->release();
  indexBuffer->release();
  for (MTL::Buffer* buffer : uniformsBuffers) {
	buffer->release();
  }
}

void TileRenderPass::loadTextures(GameScene* scene)
{
  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ResourceBundle::absolutePath("86570436012", ""), pt);
  const boost::property_tree::ptree tilesArr = pt.get_child("tiles");
  // Iterator to iterate between array items (aka tiles)
  for (boost::property_tree::ptree::const_iterator arrItemsIterator = tilesArr.begin(); arrItemsIterator != tilesArr.end(); ++arrItemsIterator) {
	uint16_t instanceId {};
	std::string textureName {};
	TileInstanceData data {};
	// Iterator to move between fields inside a specific array item (aka tile)
	for (boost::property_tree::ptree::const_iterator arrItemIterator = arrItemsIterator->second.begin(); arrItemIterator != arrItemsIterator->second.end(); ++arrItemIterator) {
	  const std::string key = arrItemIterator->first;
	  if (key.compare("instanceId") == 0) {
		instanceId = boost::lexical_cast<uint16_t>(arrItemIterator->second.data());
	  } else if (key.compare("textureName") == 0) {
		textureName = arrItemIterator->second.data();
		const uint16_t textureIndex = makeTexturesFromArt(("tile/" + textureName).c_str(), "art");
		data.textureIndex = textureIndex;
	  } else if (key.compare("shouldFlip") == 0) {
		bool shouldFlip = boost::lexical_cast<bool>(arrItemIterator->second.data());
		data.shouldFlip = shouldFlip;
		instanceIdToData.insert(std::make_pair(instanceId, data));
	  } else
		throw std::runtime_error("Unknown tile array format");
	}
  }
}

const uint16_t TileRenderPass::makeTexturesFromArt(const char * name, const char * type) const
{
  TextureController& txController = TextureController::instance(device);
  if (txController.textureExist(name, type)) {
	std::cout << "Texture already loaded. Texture name: " << name << std::endl;
	const uint16_t textureIndex = txController.textureIndexByName(name);
	return textureIndex;
  }
  PixelData pd = ArtImporter::importArt(name, type);
  // Tile art only has one frame
  // Tile art only uses first palette
  const std::vector<uint8_t> bgras = pd.bgraFrameFromPalette(0, 0);
  const uint16_t textureIndex = txController.loadTexture(name, pd.frames().at(0).imgHeight, pd.frames().at(0).imgWidth, bgras.data());
  return textureIndex;
}

void TileRenderPass::buildPipelineStates(MTL::Library* library)
{
  // Make rendering pipeline
  renderPipelineState = Pipelines::newPSO(device, library, NS::String::string("tileVertex", NS::UTF8StringEncoding), NS::String::string("tileFragment", NS::UTF8StringEncoding), false);
  
  // Make compute pipeline
  NS::String* kernelFnName = NS::String::string("cullTilesAndEncodeCommands", NS::UTF8StringEncoding);
  tileVisibilityKernelFn = library->newFunction(kernelFnName);
  computePipelineState = Pipelines::newComputePSO(device, library, kernelFnName);
}

void TileRenderPass::buildVertexBuffers(GameScene* scene)
{
  Tile* tile = scene->getTile();
  flippedVertexBuffer = device->newBuffer(tile->getFlippedVertexData().data(), tile->getFlippedVertexData().size() * sizeof(VertexData), MTL::ResourceStorageModeShared);
  vertexBuffer = device->newBuffer(tile->getVertexData().data(), tile->getVertexData().size() * sizeof(VertexData), MTL::ResourceStorageModeShared);
  indexBuffer = device->newBuffer(tile->getIndices().data(), tile->getIndices().size() * sizeof(uint16_t), MTL::ResourceStorageModeShared);
}

void TileRenderPass::buildDepthStencilState()
{
  MTL::DepthStencilDescriptor* depthStencilDesc = MTL::DepthStencilDescriptor::alloc()->init();
  depthStencilDesc->setDepthCompareFunction(MTL::CompareFunctionLess);
  depthStencilDesc->setDepthWriteEnabled(true);
  depthStencilState = device->newDepthStencilState(depthStencilDesc);
  depthStencilDesc->release();
}

void TileRenderPass::buildIndirectCommandBuffer()
{
  // Make indirect command buffer
  MTL::IndirectCommandBufferDescriptor* icbDescriptor = MTL::IndirectCommandBufferDescriptor::alloc()->init();
  icbDescriptor->setCommandTypes(MTL::IndirectCommandTypeDrawIndexed);
  // Indicate that buffers will be set for each command in the indirect command buffer.
  icbDescriptor->setInheritBuffers(false);
  // Indicate that a maximum of 3 buffers will be set for each command.
  icbDescriptor->setMaxVertexBufferBindCount(25);
  icbDescriptor->setMaxFragmentBufferBindCount(25);
#if defined TARGET_MACOS || defined(__IPHONE_13_0)
  // Indicate that the render pipeline state object will be set in the render command encoder
  // (not by the indirect command buffer).
  // On iOS, this property only exists on iOS 13 and later.  Earlier versions of iOS did not
  // support settings pipelinestate within an indirect command buffer, so indirect command
  // buffers always inherited the pipeline state.
  icbDescriptor->setInheritPipelineState(true);
#endif
  
  // Create indirect command buffer using private storage mode; since only the GPU will
  // write to and read from the indirect command buffer, the CPU never needs to access the
  // memory
  indirectCommandBuffer = device->newIndirectCommandBuffer(icbDescriptor, RenderingSettings::NumOfTilesPerSector, MTL::ResourceStorageModeShared);
  
  icbDescriptor->release();
  
  indirectCommandBuffer->setLabel(NS::String::string("Tile ICB", NS::UTF8StringEncoding));
  
  // Make ICB Argument buffer
  // Argument buffer containing the indirect command buffer encoded in the kernel
  MTL::ArgumentEncoder* argumentEncoder = tileVisibilityKernelFn->newArgumentEncoder(BufferIndices::ICBBuffer);
  icbArgumentBuffer = device->newBuffer(argumentEncoder->encodedLength(), MTL::ResourceStorageModeShared);
  icbArgumentBuffer->setLabel(NS::String::string("Tile ICB Argument Buffer", NS::UTF8StringEncoding));
  argumentEncoder->setArgumentBuffer(icbArgumentBuffer, 0);
  argumentEncoder->setIndirectCommandBuffer(indirectCommandBuffer, BufferIndices::ICBArgumentsBuffer);

  argumentEncoder->release();
}

void TileRenderPass::draw(MTL::CommandBuffer* commandBuffer, CA::MetalDrawable* drawable, MTL::Texture* depthTexture, GameScene* scene, float_t deltaTime, const uint16_t frame)
{
  // Encode command to reset the indirect command buffer
  MTL::BlitCommandEncoder* resetBlitEncoder = commandBuffer->blitCommandEncoder();
  resetBlitEncoder->setLabel(NS::String::string("Tile reset ICB Blit Encoder", NS::UTF8StringEncoding));
  resetBlitEncoder->resetCommandsInBuffer(indirectCommandBuffer, NS::Range(0, RenderingSettings::NumOfTilesPerSector));
  resetBlitEncoder->endEncoding();
  
  // Encode commands to determine visibility of tiles using a compute kernel
  MTL::ComputeCommandEncoder* computeEncoder = commandBuffer->computeCommandEncoder();
  computeEncoder->setLabel(NS::String::string("Tile Visibility Kernel", NS::UTF8StringEncoding));
  computeEncoder->setComputePipelineState(computePipelineState);
  
  scene->getTile()->update(deltaTime);
  
  // Since we are using instanced rendering, we have to use triple-buffering for instance data buffers to avoid race conditions between CPU and GPU
  MTL::Buffer* instanceDataBuffer = instanceDataBuffers.at(frame);
  TileInstanceData* instanceData = reinterpret_cast<TileInstanceData*>(instanceDataBuffer->contents());
  // Translate entire sector to ensure that camera - which located at (0, 0) - points at a center of a sector
  // We want to look at (32, 32), because that's where the player's character pops up at the start of the game
  const float_t baseRowOffset = .0f;
  const float_t baseColumnOffset = .0f;
  Tile* tile = scene->getTile();
  for (size_t i = 0; i < RenderingSettings::NumOfTilesPerSector; ++i) {
	const float_t rowOffset = baseRowOffset + (float_t) (i % (RenderingSettings::NumOfTilesPerSector / RenderingSettings::NumOfTilesPerRow));
	const float_t columnOffset = baseColumnOffset + (float_t) (i / (RenderingSettings::NumOfTilesPerSector / RenderingSettings::NumOfTilesPerRow));
	instanceData[i].instanceTransform = Math::getInstance().translation(rowOffset * 2.0f, 0.0f, columnOffset * 2.0f);
	const std::unordered_map<uint16_t, TileInstanceData>::const_iterator iterator = instanceIdToData.find(i);
	if (iterator == instanceIdToData.end())
	  throw std::runtime_error("Texture index not found for instanceId. instanceId = " + std::to_string(i));
	instanceData[i].textureIndex = iterator->second.textureIndex;
	instanceData[i].shouldFlip = iterator->second.shouldFlip;
  }
#if defined(TARGET_OSX)
  instanceDataBuffer->didModifyRange(NS::Range::Make(0, instanceDataBuffer->length()));
#endif
  
  Uniforms& uf = Uniforms::getInstance();
  uf.setModelMatrix(tile->modelMatrix());
  // Since we are using instanced rendering, we have to use triple-buffering for Uniforms buffers to avoid race conditions between CPU and GPU
  MTL::Buffer* ufBuffer = uniformsBuffers.at(frame);
  memcpy(ufBuffer->contents(), &uf, Alignment::roundUpToNextMultipleOf16(sizeof(Uniforms)));
#if defined(TARGET_OSX)
  ufBuffer->didModifyRange(NS::Range(0, ufBuffer->length()));
#endif
  
  computeEncoder->setBuffer(ufBuffer, 0, BufferIndices::UniformsBuffer);
  computeEncoder->setBuffer(vertexBuffer, 0, BufferIndices::VertexBuffer);
  computeEncoder->setBuffer(flippedVertexBuffer, 0, BufferIndices::FlippedVertexBuffer);
  computeEncoder->setBuffer(indexBuffer, 0, BufferIndices::IndexBuffer);
  computeEncoder->setBuffer(instanceDataBuffer, 0, BufferIndices::InstanceDataBuffer);
  
  computeEncoder->useResource(ufBuffer, MTL::ResourceUsageRead);
  computeEncoder->useResource(vertexBuffer, MTL::ResourceUsageRead);
  computeEncoder->useResource(flippedVertexBuffer, MTL::ResourceUsageRead);
  computeEncoder->useResource(indexBuffer, MTL::ResourceUsageRead);
  computeEncoder->useResource(instanceDataBuffer, MTL::ResourceUsageRead);
  
  computeEncoder->setBuffer(icbArgumentBuffer, 0, BufferIndices::ICBBuffer);
  computeEncoder->setBuffer(materialBuffer, 0, BufferIndices::TextureBuffer);
  // Call useResource on '_indirectCommandBuffer' which indicates to Metal that the kernel will
  // access '_indirectCommandBuffer'.  It is necessary because the app cannot directly set
  // '_indirectCommandBuffer' in 'computeEncoder', but, rather, must pass it to the kernel via
  // an argument buffer which indirectly contains '_indirectCommandBuffer'.
  computeEncoder->useResource(indirectCommandBuffer, MTL::ResourceUsageWrite);
  computeEncoder->useHeap(TextureController::instance(device).heap());
  uint64_t threadExecutionWidth = computePipelineState->threadExecutionWidth();
  computeEncoder->dispatchThreads(MTL::Size(RenderingSettings::NumOfTilesPerSector, 1, 1), MTL::Size(threadExecutionWidth, 1, 1));
  computeEncoder->endEncoding();
  
  // Encode command to optimize the indirect command buffer after encoding
  MTL::BlitCommandEncoder* optimizeBlitEncoder = commandBuffer->blitCommandEncoder();
  optimizeBlitEncoder->setLabel(NS::String::string("Tile Optimize ICB Blit Encoder", NS::UTF8StringEncoding));
  optimizeBlitEncoder->optimizeIndirectCommandBuffer(indirectCommandBuffer, NS::Range(0, RenderingSettings::NumOfTilesPerSector));
  optimizeBlitEncoder->endEncoding();
  
  MTL::RenderPassDescriptor* rpd = MTL::RenderPassDescriptor::alloc()->init();
  rpd->colorAttachments()->object(0)->setTexture(drawable->texture());
  rpd->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionClear);
  rpd->colorAttachments()->object(0)->setClearColor(MTL::ClearColor::Make(0.0f, 0.0f, 1.0f, 1.0));
  
  MTL::RenderPassDepthAttachmentDescriptor* depthAttachmentDescriptor = MTL::RenderPassDepthAttachmentDescriptor::alloc()->init();
  depthAttachmentDescriptor->setTexture(depthTexture);
  rpd->setDepthAttachment(depthAttachmentDescriptor);
  depthAttachmentDescriptor->release();

  MTL::RenderCommandEncoder* renderEncoder = commandBuffer->renderCommandEncoder(rpd);
  rpd->release();
  renderEncoder->setLabel(NS::String::string("Tile Render Encoder", NS::UTF8StringEncoding));
  renderEncoder->setRenderPipelineState(renderPipelineState);
  renderEncoder->setDepthStencilState(depthStencilState);
  renderEncoder->executeCommandsInBuffer(indirectCommandBuffer, NS::Range(0, RenderingSettings::NumOfTilesPerSector));
  renderEncoder->endEncoding();
}
