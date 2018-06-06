//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/MouseButton.hpp"
#include "xr/debug.hpp"
#include "SDL_mouse.h"
#include <algorithm>

namespace xr
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
  SDL_BUTTON_X2,

  // Wheels don't have SDL_BUTTON representation; should not come through this way.
  0,
  0,
  0,
  0,
};

//==============================================================================
Type TranslateNative(int mb)
{
  const auto iFind = std::find(kNative, kNative + kCount, mb);
  const auto result = static_cast<Type>(iFind - kNative);
  XR_ASSERTMSG(MouseButton, result != MouseButton::Unknown,
    ("Cannot translate mouse button %d.", mb));
  return result;
}

} // MouseButton
} // XR
