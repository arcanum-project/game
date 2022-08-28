//
//  TouchableMTKView.m
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 7/26/22.
//

#import <UIKit/UIKit.h>
#import "MetalKit/MTKView.h"
#import "InputControllerAdapter.h"

@interface TouchableMTKView : MTKView
  
@end

@implementation TouchableMTKView {
  InputControllerAdapter * _inputControllerAdapter;
}

- (instancetype)init {
  _inputControllerAdapter = [InputControllerAdapter alloc];
  return [super init];
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
  MTKView * const view = (MTKView * const) touches.anyObject.view;
  const CGPoint touchPoint = [touches.anyObject locationInView:view];
  [[InputControllerAdapter instance]setCoordinates:touchPoint];
}

@end
