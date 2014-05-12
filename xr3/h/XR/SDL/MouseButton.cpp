//
// Nuclear Heart Games
// XRhodes
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#include <algorithm>
#include <SDL2/SDL_mouse.h>
#include "MouseButton.hpp"

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
