//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <algorithm>
#include <SDL_mouse.h>
#include "XR/MouseButton.hpp"

namespace XR {

//==============================================================================
const int karMouseButtonNative[] =
{
  SDL_BUTTON_LEFT,
  SDL_BUTTON_RIGHT,
  SDL_BUTTON_MIDDLE,
  SDL_BUTTON_X1,
  SDL_BUTTON_X2
};

//==============================================================================
MouseButton TranslateMouseButtonNative(int mb)
{
  const int*  iFind(std::find(karMouseButtonNative, karMouseButtonNative +
    kMouseButtonCount, mb));
  return static_cast<MouseButton>(iFind - karMouseButtonNative);
}

} // XR
