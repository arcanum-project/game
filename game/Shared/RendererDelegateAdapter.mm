//
//  RendererAdapter.m
//  triangle-with-animation
//
//  Created by Dmitrii Belousov on 7/1/22.
//

#import "RendererDelegateAdapter.h"
#import "Renderer.hpp"

@implementation RendererDelegateAdapter
{
  MTKView * _pView;
  Renderer * _pRenderer;
}

-(nonnull instancetype)initWithMetalKitView:(MTKView *)pView;
{
  self = [super init];
  _pView = pView;
  _pRenderer = new Renderer((__bridge MTL::Device * )pView.device);
  return self;
}

-(void)dealloc
{
  delete _pRenderer;
}

- (void)drawInMTKView:(nonnull MTKView *)view {
  _pRenderer->drawFrame((__bridge CA::MetalDrawable const *)view.currentDrawable, (__bridge MTL::Texture const *)view.depthStencilTexture);
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size {
  _pRenderer->drawableSizeWillChange(size.width, size.height);
}

@end
