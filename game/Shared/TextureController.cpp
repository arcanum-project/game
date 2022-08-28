//
//  TextureController.cpp
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 8/3/22.
//

#include "TextureController.hpp"

TextureController & TextureController::instance(MTL::Device * const pDevice) {
  static TextureController instance(pDevice);
  return instance;
}

TextureController::TextureController(MTL::Device * const pDevice)
: _pDevice(pDevice),
  _pTextures(std::vector<MTL::Texture *>()),
  _textureIndices(std::unordered_map<std::string, uint16_t>()),
  _pHeap(nullptr)
{}

TextureController::~TextureController() {
  _pHeap->release();
  for (MTL::Texture * const pTexture : _pTextures) {
	pTexture->release();
  }
}

MTL::Texture * const TextureController::makeTexture(const char * imgName, const char * imgType) const {
  const PixelData pPixelData = BMPImporter::import(imgName, imgType);
  
  MTL::TextureDescriptor * const pTextureDescriptor = MTL::TextureDescriptor::alloc()->init();
  pTextureDescriptor->setTextureType( MTL::TextureType2D );
  pTextureDescriptor->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
  pTextureDescriptor->setWidth(pPixelData.imgWidth);
  pTextureDescriptor->setHeight(pPixelData.imgHeight);
  pTextureDescriptor->setStorageMode( MTL::StorageModeShared );
  pTextureDescriptor->setUsage( MTL::ResourceUsageSample | MTL::ResourceUsageRead );
  MTL::Texture * const pTexture = _pDevice->newTexture(pTextureDescriptor);
  
  const MTL::Region region = MTL::Region(0, 0, 0, pPixelData.imgWidth, pPixelData.imgHeight, 1);
  const uint32_t bytesPerRow = 4 * pPixelData.imgWidth;
  
  pTexture->replaceRegion(region, 0, pPixelData.pixels.data(), bytesPerRow);
  
  pTexture->setLabel(NS::String::string(imgName, NS::UTF8StringEncoding));
  
  pTextureDescriptor->release();
  
  return pTexture;
}

/**
 Loads a texture and returns its index in texture indicies array.
 */
const uint16_t TextureController::loadTexture(const char * imgName, const char * imgType) {
  MTL::Texture * pTexture {nullptr};
  const std::unordered_map<std::string, uint16_t>::const_iterator textureIndexIterator = _textureIndices.find(imgName);
  if (textureIndexIterator != _textureIndices.end()) {
	std::cout << "Texture already loaded. Texture name: " << imgName << std::endl;
	return textureIndexIterator->second;
  } else {
	pTexture = makeTexture(imgName, imgType);
	const uint16_t textureIndex = _pTextures.size();
	_textureIndices.insert(std::make_pair(imgName, textureIndex));
	_pTextures.push_back(pTexture);
	return textureIndex;
  }
}

/**
 Create a heap large enough to contain all textures
 */
MTL::Heap * const TextureController::makeHeap() {
  MTL::HeapDescriptor * const pHeapDescriptor = MTL::HeapDescriptor::alloc()->init();
  pHeapDescriptor->setStorageMode(MTL::StorageModePrivate);
//  pHeapDescriptor->setStorageMode(MTL::StorageModeShared);
  pHeapDescriptor->setSize(0);
  
  // Build a descriptor for each texture and calculate the size required to store all textures in the heap
  for (MTL::Texture * tx : _pTextures) {
	// Create a descriptor using the texture's properties
	MTL::TextureDescriptor * const txDesc = newDescriptorFromTexture(tx, pHeapDescriptor->storageMode());
	
	// Determine the size required for the heap for the given descriptor
	MTL::SizeAndAlign sizeAndAlign = _pDevice->heapTextureSizeAndAlign(txDesc);
	
	// Align the size so that more resources will fit in the heap after this texture
	sizeAndAlign.size += (sizeAndAlign.size & (sizeAndAlign.align - 1)) + sizeAndAlign.align;
	
	// Accumulate the size required to store this buffer in the heap
	pHeapDescriptor->setSize(pHeapDescriptor->size() + sizeAndAlign.size);
	
	txDesc->release();
  }
  
  _pHeap = _pDevice->newHeap(pHeapDescriptor);
  
  pHeapDescriptor->release();
  
  return _pHeap;
}

MTL::TextureDescriptor * const TextureController::newDescriptorFromTexture(MTL::Texture * const pTexture, const MTL::StorageMode storageMode) const {
  MTL::TextureDescriptor * const desc = MTL::TextureDescriptor::alloc()->init();
  desc->setTextureType(pTexture->textureType());
  desc->setPixelFormat(pTexture->pixelFormat());
  desc->setWidth(pTexture->width());
  desc->setHeight(pTexture->height());
  desc->setDepth(pTexture->depth());
  desc->setMipmapLevelCount(pTexture->mipmapLevelCount());
  desc->setArrayLength(pTexture->arrayLength());
  desc->setSampleCount(pTexture->sampleCount());
  desc->setStorageMode(storageMode);
  return desc;
}

/**
 Move loaded textures into heap
 */
void TextureController::moveTexturesToHeap(MTL::CommandQueue * const pCommandQueue) {
  // Create a command buffer and blit encoder to copy data from the existing resources to
  // the new resources created from the heap
  MTL::CommandBuffer * const pCommandBuffer = pCommandQueue->commandBuffer();
  pCommandBuffer->setLabel(NS::String::string("Heap copy command buffer", NS::UTF8StringEncoding));
  
  MTL::BlitCommandEncoder * const pBlitCommandEnc = pCommandBuffer->blitCommandEncoder();
  pBlitCommandEnc->setLabel(NS::String::string("Heap transfer blit encoder", NS::UTF8StringEncoding));
  
  // Create new textures from the heap and copy the contents of the existing textures to
  // the new textures
  for (NS::UInteger txI = 0; txI < _pTextures.size(); ++txI) {
	// Create a descriptor using the texture's properties
	MTL::TextureDescriptor * const pTxDesc = newDescriptorFromTexture(_pTextures.at(txI), _pHeap->storageMode());
	
	// Create a texture from the heap
	MTL::Texture * pHeapTexture = _pHeap->newTexture(pTxDesc);
	pHeapTexture->setLabel(_pTextures.at(txI)->label());
	
	// Blit every slice of every level from the existing texture to the new texture
	MTL::Region region = MTL::Region(0, 0, _pTextures.at(txI)->width(), _pTextures.at(txI)->height());
	
	pBlitCommandEnc->copyFromTexture(_pTextures.at(txI), 0, 0, region.origin, region.size, pHeapTexture, 0, 0, region.origin);
	
	// Replace the existing texture with the new texture
	_pTextures.at(txI)->release();
	_pTextures.at(txI) = pHeapTexture;
	
	pTxDesc->release();
  }
  
  pBlitCommandEnc->endEncoding();
  pCommandBuffer->commit();
}
