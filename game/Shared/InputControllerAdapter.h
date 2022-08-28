//
//  InputControllerAdapter.h
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 7/30/22.
//

#ifndef InputControllerAdapter_h
#define InputControllerAdapter_h

#import "CoreGraphics/CoreGraphics.h"

@interface InputControllerAdapter : NSObject {
  float_t _xCoordinate;
  float_t _yCoordinate;
};

+(instancetype)instance;

-(void)setCoordinates:(CGPoint)point;

-(const float_t)_xCoordinate;

-(const float_t)_yCoordinate;

@end

#endif /* InputControllerAdapter_h */
