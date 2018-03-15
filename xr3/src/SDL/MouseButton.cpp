//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <algorithm>
#include "SDL_mouse.h"
#include "XR/MouseButton.hpp"

namespace XR
{
namespace MouseButton
{
//==============================================================================
const int kNative[] =
{
  SDL_BUTTON_LEFT,
  SDL_BUTTON_RIGHT,
  SDL_BUTTON_MIDDLE,
  SDL_BUTTON_X1,
  SDL_BUTTON_X2
};

//==============================================================================
Type TranslateNative(int mb)
{
  auto iFind = std::find(kNative, kNative + kCount, mb);
  return static_cast<Type>(iFind - kNative);
}

} // MouseButton
} // XR
