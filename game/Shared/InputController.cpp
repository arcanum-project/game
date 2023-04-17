//

#include "InputController.h"

InputController::InputController()
: xNormalizedScreen(0.f),
  yNormalizedScreen(0.f)
{
}

InputController& InputController::getInstance()
{
  static InputController instance;
  return instance;
}
