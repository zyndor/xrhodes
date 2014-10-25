//
// Nuclear Heart Games
// XRhodes
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#include <algorithm>
#include <SDL2/SDL_scancode.h>
#include "KeyCode.hpp"

namespace XR
{

//==============================================================================
const int karKeyCodeNative[] =
{
  SDL_SCANCODE_LSHIFT, //225,
  SDL_SCANCODE_LCTRL, //224,
  SDL_SCANCODE_LALT, //226, /**< alt, option */
  SDL_SCANCODE_RSHIFT, //229,
  SDL_SCANCODE_RCTRL, //228,
  SDL_SCANCODE_RALT, //230, /**< alt gr, option */
  
  SDL_SCANCODE_LEFT, //80,
  SDL_SCANCODE_UP, //82,
  SDL_SCANCODE_DOWN, //81,
  SDL_SCANCODE_RIGHT, //79,

  SDL_SCANCODE_ESCAPE, //41,
  SDL_SCANCODE_BACKSPACE, //42,
  SDL_SCANCODE_TAB, //43,
  SDL_SCANCODE_CAPSLOCK, //57
  SDL_SCANCODE_RETURN, //40,
  SDL_SCANCODE_SPACE, //44,

  SDL_SCANCODE_PAGEUP, //75,
  SDL_SCANCODE_PAGEDOWN, //78,
  SDL_SCANCODE_HOME, //74,
  SDL_SCANCODE_END, //77,
  SDL_SCANCODE_INSERT, //73,
  SDL_SCANCODE_DELETE, //76,
  
  SDL_SCANCODE_PAUSE,
  
  SDL_SCANCODE_F1, //58,
  SDL_SCANCODE_F2, //59,
  SDL_SCANCODE_F3, //60,
  SDL_SCANCODE_F4, //61,
  SDL_SCANCODE_F5, //62,
  SDL_SCANCODE_F6, //63,
  SDL_SCANCODE_F7, //64,
  SDL_SCANCODE_F8, //65,
  SDL_SCANCODE_F9, //66,
  SDL_SCANCODE_F10, //67,
  SDL_SCANCODE_F11, //68,
  SDL_SCANCODE_F12, //69,

  SDL_SCANCODE_1, //30,
  SDL_SCANCODE_2, //31,
  SDL_SCANCODE_3, //32,
  SDL_SCANCODE_4, //33,
  SDL_SCANCODE_5, //34,
  SDL_SCANCODE_6, //35,
  SDL_SCANCODE_7, //36,
  SDL_SCANCODE_8, //37,
  SDL_SCANCODE_9, //38,
  SDL_SCANCODE_0, //39,
  
  SDL_SCANCODE_A, //4,
  SDL_SCANCODE_B, //5,
  SDL_SCANCODE_C, //6,
  SDL_SCANCODE_D, //7,
  SDL_SCANCODE_E, //8,
  SDL_SCANCODE_F, //9,
  SDL_SCANCODE_G, //10,
  SDL_SCANCODE_H, //11,
  SDL_SCANCODE_I, //12,
  SDL_SCANCODE_J, //13,
  SDL_SCANCODE_K, //14,
  SDL_SCANCODE_L, //15,
  SDL_SCANCODE_M, //16,
  SDL_SCANCODE_N, //17,
  SDL_SCANCODE_O, //18,
  SDL_SCANCODE_P, //19,
  SDL_SCANCODE_Q, //20,
  SDL_SCANCODE_R, //21,
  SDL_SCANCODE_S, //22,
  SDL_SCANCODE_T, //23,
  SDL_SCANCODE_U, //24,
  SDL_SCANCODE_V, //25,
  SDL_SCANCODE_W, //26,
  SDL_SCANCODE_X, //27,
  SDL_SCANCODE_Y, //28,
  SDL_SCANCODE_Z, //29,

  SDL_SCANCODE_GRAVE, //53,
  SDL_SCANCODE_LEFTBRACKET, //47,
  SDL_SCANCODE_RIGHTBRACKET, //48,
  SDL_SCANCODE_SEMICOLON, //51,
  SDL_SCANCODE_APOSTROPHE, //52,
  SDL_SCANCODE_NONUSHASH, //49
  SDL_SCANCODE_MINUS, //45,
  SDL_SCANCODE_EQUALS, //46,
  SDL_SCANCODE_COMMA, //54,
  SDL_SCANCODE_PERIOD, //55,
  SDL_SCANCODE_SLASH, //56,
  SDL_SCANCODE_BACKSLASH, //49,
  
  SDL_SCANCODE_AC_BACK
};

//==============================================================================
KeyCode TranslateKeyCodeNative(int kc)
{
  const int*  iFind(std::find(karKeyCodeNative, karKeyCodeNative + kKeyCount, kc));
  return static_cast<KeyCode>(iFind - karKeyCodeNative);
}

} // XR
