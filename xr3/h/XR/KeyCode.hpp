//
// Nuclear Heart Games
// XRhodes
//
// KeyCode.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    17/01/2014
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_KEYCODE_HPP
#define XR_KEYCODE_HPP

namespace XR
{

//==============================================================================
enum  KeyCode
{
  K_LSHIFT,
  K_LCTRL,
  K_LALT,
  K_RSHIFT,
  K_RCTRL,
  K_RALT,
  
  K_LEFT,
  K_UP,
  K_DOWN,
  K_RIGHT,
  
  K_ESC,
  K_BACKSPACE,
  K_TAB,
  K_CAPSLOCK,
  K_ENTER,
  K_SPACE,
  
  K_INS,
  K_HOME,
  K_PGUP,
  K_PGDOWN,
  K_END,
  K_DEL,

  K_PAUSE,
  
  K_F1,
  K_F2,
  K_F3,
  K_F4,
  K_F5,
  K_F6,
  K_F7,
  K_F8,
  K_F9,
  K_F10,
  K_F11,
  K_F12,
                
  K_1,
  K_2,
  K_3,
  K_4,
  K_5,
  K_6,
  K_7,
  K_8,
  K_9,
  K_0,
  
  K_A,
  K_B,
  K_C,
  K_D,
  K_E,
  K_F,
  K_G,
  K_H,
  K_I,
  K_J,
  K_K,
  K_L,
  K_M,
  K_N,
  K_O,
  K_P,
  K_Q,
  K_R,
  K_S,
  K_T,
  K_U,
  K_V,
  K_W,
  K_X,
  K_Y,
  K_Z,
  
  K_BACKTICK,   //`
  K_LBRACKET,   //[
  K_RBRACKET,   //]
  K_SEMICOLON,  //;
  K_APOSTROPHE, //'
  K_HASH,       //#
  K_MINUS,      //-
  K_EQUALS,     //=
  K_COMMA,      //,
  K_PERIOD,     //.
  K_SLASH,      //.
  K_BACKSLASH,  // \ reverse solidus?

  K_BACK,
  K_MENU,
  K_OK,
  
  kKeyCount,
  K_UNKNOWN = kKeyCount
};

//==============================================================================
extern const int karKeyCodeNative[kKeyCount];

//==============================================================================
KeyCode TranslateKeyCodeNative(int kc);

} // XR

#endif // XR_KEYCODE_HPP
