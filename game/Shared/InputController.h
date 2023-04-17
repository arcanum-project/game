//

#pragma once

class InputController
{
public:
  static InputController& getInstance();
  InputController(InputController const&) = delete;
  void operator=(InputController const&) = delete;
  
  inline void setCoordinates(float x, float y)
  {
	xNormalizedScreen = x;
	yNormalizedScreen = y;
  }
  
  inline float getXCoordinate() const { return xNormalizedScreen; }
  
  inline float getYCoordinate() const { return yNormalizedScreen; }
  
private:
  InputController();
  ~InputController() = default;
  
  float xNormalizedScreen;
  float yNormalizedScreen;
};
