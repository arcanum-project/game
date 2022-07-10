//
//  RendererAdapter.m
//  triangle-with-animation
//
//  Created by Dmitrii Belousov on 7/1/22.
//

#import "RendererDelegateAdapter.h"
#import "Renderer.hpp"
#import "Math.hpp"
#import "Uniforms.hpp"

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
  _pRenderer->drawFrame((__bridge CA::MetalDrawable *)view.currentDrawable);
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size {
  float_t aspect = view.bounds.size.height / view.bounds.size.width;
  glm::mat4x4 projectionMat = Math::getInstance().perspective(70, 0.1, 100, aspect);
  Uniforms::getInstance().setProjectionMatrix(projectionMat);
}

@end
