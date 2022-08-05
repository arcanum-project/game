//
//  TextureController.hpp
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 8/3/22.
//

#ifndef TextureController_hpp
#define TextureController_hpp

#include "Metal/Metal.hpp"
#include <vector>

#include "BMPImporter.hpp"
#include "PixelData.hpp"

class TextureController {
public:
  static const TextureController & instance(MTL::Device * const pDevice);
  TextureController(TextureController const &) = delete;
  void operator=(TextureController const &) = delete;
  
  inline MTL::Texture * const makeTexture(const char * imgName, const char * imgType) const {
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
	
	pTextureDescriptor->release();
	
	return pTexture;
  }
  
private:
  TextureController(MTL::Device * const pDevice);
  ~TextureController();
  
  MTL::Device * const _pDevice;
};

#endif /* TextureController_hpp */
