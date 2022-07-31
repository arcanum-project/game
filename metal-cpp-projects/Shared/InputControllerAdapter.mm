//
//  InputControllerAdapter.m
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 7/30/22.
//

#import "InputControllerAdapter.h"
#import "InputControllerBridge.h"
#import "Uniforms.hpp"

@implementation InputControllerAdapter

extern "C" const float_t xCoordinate() {
  return [[InputControllerAdapter instance] _xCoordinate];
}

extern "C" const bool setCoordinates(float x, float y) {
  [[InputControllerAdapter instance] setCoordinates:CGPoint { x, y }];
  return true;
}

extern "C" const float_t yCoordinate() {
  return [[InputControllerAdapter instance] _yCoordinate];
}

/*
 Singleton.
 From here: https://stackoverflow.com/questions/7568935/how-do-i-implement-an-objective-c-singleton-that-is-compatible-with-arc
 */
+ (instancetype)instance {
  static InputControllerAdapter * instance = nil;
  static dispatch_once_t onceToken;
  dispatch_once(&onceToken, ^{
	instance = [[InputControllerAdapter alloc] init];
	CGPoint defaultPoint = CGPoint { 0.0f, 0.0f };
	[instance setCoordinates:defaultPoint];
  });
  return instance;
}

/**
  Save coordinates of a touch.
 */
- (void)setCoordinates:(const CGPoint)point {
  _xCoordinate = point.x;
  _yCoordinate = point.y;
}

- (const float_t)_xCoordinate {
  return _xCoordinate;
}

- (const float_t)_yCoordinate {
  return _yCoordinate;
}

@end
