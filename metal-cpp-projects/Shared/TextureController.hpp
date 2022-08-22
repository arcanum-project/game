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
#include <unordered_map>
#include <string>

#include "BMPImporter.hpp"
#include "PixelData.hpp"

class TextureController {
public:
  static TextureController & instance(MTL::Device * const pDevice);
  TextureController(TextureController const &) = delete;
  void operator=(TextureController const &) = delete;
  
  inline std::vector<MTL::Texture *> textures() const { return _pTextures; };
  inline MTL::Heap * const heap() const { return _pHeap; }
  const uint16_t loadTexture(const char * imgName, const char * imgType);
  MTL::Heap * const makeHeap();
  void moveTexturesToHeap(MTL::CommandQueue * const pCommandQueue);
  
private:
  TextureController(MTL::Device * const pDevice);
  ~TextureController();
  MTL::Texture * const makeTexture(const char * imgName, const char * imgType) const;
  MTL::TextureDescriptor * const newDescriptorFromTexture(MTL::Texture * const pTexture, const MTL::StorageMode storageMode) const;
  
  MTL::Device * const _pDevice;
  std::vector<MTL::Texture *> _pTextures;
  std::unordered_map<std::string, uint16_t> _textureIndices;
  MTL::Heap * _pHeap;
};

#endif /* TextureController_hpp */
