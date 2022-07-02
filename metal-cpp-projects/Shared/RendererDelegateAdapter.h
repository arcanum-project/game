//
//  RendererAdapter.h
//  triangle-with-animation
//
//  Created by Dmitrii Belousov on 7/1/22.
//

#import "MetalKit/MTKView.h"

@interface RendererDelegateAdapter : NSObject<MTKViewDelegate>

-(nonnull instancetype)initWithMetalKitView:(nonnull MTKView *)pView;

@end
