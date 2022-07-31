//
//  InputControllerBridge.h
//  moving-isometric-camera
//
//  Created by Dmitrii Belousov on 7/30/22.
//

#ifndef InputControllerBridge_h
#define InputControllerBridge_h

extern "C" const float xCoordinate(void);
extern "C" const float yCoordinate(void);
extern "C" const bool setCoordinates(float, float);

#endif /* InputControllerBridge_h */
