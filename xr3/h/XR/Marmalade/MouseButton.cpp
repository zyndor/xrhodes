//
// Nuclear Heart Games
// XRhodes
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#include <algorithm>
#include <s3ePointer.h>
#include "MouseButton.hpp"

namespace XR {

//==============================================================================
const int karMouseButtonNative[] =
{
  S3E_POINTER_BUTTON_LEFTMOUSE,
  S3E_POINTER_BUTTON_RIGHTMOUSE,
  S3E_POINTER_BUTTON_MIDDLEMOUSE,
  S3E_POINTER_BUTTON_MOUSEWHEELUP,
  S3E_POINTER_BUTTON_MOUSEWHEELDOWN,
};

//==============================================================================
MouseButton TranslateMouseButtonNative(int mb)
{
  const int*  iFind(std::find(karMouseButtonNative, karMouseButtonNative +
    kMouseButtonCount, mb));
  return static_cast<MouseButton>(iFind - karMouseButtonNative);
}

} // XR
