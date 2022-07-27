//
//  TouchableMTKView.m
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 7/26/22.
//

#import <UIKit/UIKit.h>
#import "MetalKit/MTKView.h"

@interface TouchableMTKView : MTKView
  
@end

@implementation TouchableMTKView

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
  NSLog(@"touched!");
}

@end
