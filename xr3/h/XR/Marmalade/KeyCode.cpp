//
// Nuclear Heart Games
// XRhodes
//
// copyright (c) 2011 - 2014. All rights reserved.
//
#include <algorithm>
#include <s3eKeyboard.h>
#include "KeyCode.hpp"

namespace XR {

//==============================================================================
const int karKeyCodeNative[] =
{
  s3eKeyLeftShift, //5,    //!< Left shift key.
  s3eKeyLeftControl, //6,  //!< Left control key.
  s3eKeyLeftAlt, //99,       //!< Left Alt key.
  s3eKeyRightShift, //102,   //!< Right Shift Key.
  s3eKeyRightControl, //100, //!< Right Control Key.
  s3eKeyRightAlt, //101,     //!< Right Alt Key.

  s3eKeyLeft, //9,         //!< Key Left.
  s3eKeyUp, //10,          //!< Key Up.
  s3eKeyDown, //12,        //!< Key Down.
  s3eKeyRight, //11,       //!< Key Right.

  s3eKeyEsc, //1,          //!< Esc (cancel).
  s3eKeyBackspace, //3,    //!< Backspace.
  s3eKeyTab, //2,          //!< Tab.
  s3eKeyCapsLock, //114,     //!< Caps Lock key
  s3eKeyEnter, //4,        //!< Enter.
  s3eKeySpace, //8,        //!< Key Space.

  s3eKeyPageUp, //120,       //!< Page Up key
  s3eKeyPageDown, //121,     //!< Page Down key
  s3eKeyHome, //119,         //!< Home key
  s3eKeyKbEnd, //122,        //!< End key (on keyboard)
  s3eKeyInsert, //118,       //!< Insert key
  s3eKeyDelete, //123,       //!< Delete key

  s3eKeyF1, //49,          //!< Key F1.
  s3eKeyF2, //50,          //!< Key F2.
  s3eKeyF3, //51,          //!< Key F3.
  s3eKeyF4, //52,          //!< Key F4.
  s3eKeyF5, //53,          //!< Key F5.
  s3eKeyF6, //54,          //!< Key F6.
  s3eKeyF7, //55,          //!< Key F7.
  s3eKeyF8, //56,          //!< Key F8.
  s3eKeyF9, //57,          //!< Key F9.
  s3eKeyF10, //58,         //!< Key F10.
  s3eKeyF11, //97,           //!< Key F11.
  s3eKeyF12, //98,           //!< Key F12.

  s3eKey1, //14,           //!< Key 1 on device numberpad and on main keyboard.
  s3eKey2, //15,           //!< Key 2 on device numberpad and on main keyboard.
  s3eKey3, //16,           //!< Key 3 on device numberpad and on main keyboard.
  s3eKey4, //17,           //!< Key 4 on device numberpad and on main keyboard.
  s3eKey5, //18,           //!< Key 5 on device numberpad and on main keyboard.
  s3eKey6, //19,           //!< Key 6 on device numberpad and on main keyboard.
  s3eKey7, //20,           //!< Key 7 on device numberpad and on main keyboard.
  s3eKey8, //21,           //!< Key 8 on device numberpad and on main keyboard.
  s3eKey9, //22,           //!< Key 9 on device numberpad and on main keyboard.
  s3eKey0, //13,           //!< Key 0 on device numberpad and on main keyboard.

  s3eKeyA, //23,           //!< Key A.
  s3eKeyB, //24,           //!< Key B.
  s3eKeyC, //25,           //!< Key C.
  s3eKeyD, //26,           //!< Key D.
  s3eKeyE, //27,           //!< Key E.
  s3eKeyF, //28,           //!< Key F.
  s3eKeyG, //29,           //!< Key G.
  s3eKeyH, //30,           //!< Key H.
  s3eKeyI, //31,           //!< Key I.
  s3eKeyJ, //32,           //!< Key J.
  s3eKeyK, //33,           //!< Key K.
  s3eKeyL, //34,           //!< Key L.
  s3eKeyM, //35,           //!< Key M.
  s3eKeyN, //36,           //!< Key N.
  s3eKeyO, //37,           //!< Key O.
  s3eKeyP, //38,           //!< Key P.
  s3eKeyQ, //39,           //!< Key Q.
  s3eKeyR, //40,           //!< Key R.
  s3eKeyS, //41,           //!< Key S.
  s3eKeyT, //42,           //!< Key T.
  s3eKeyU, //43,           //!< Key U.
  s3eKeyV, //44,           //!< Key V.
  s3eKeyW, //45,           //!< Key W.
  s3eKeyX, //46,           //!< Key X.
  s3eKeyY, //47,           //!< Key Y.
  s3eKeyZ, //48,           //!< Key Z.

  s3eKeyBacktick, //103,     //!< Backtick '`' Key.
  s3eKeyLeftBracket, //110,  //!< Left Bracket '[' Key.
  s3eKeyRightBracket, //111, //!< Right Bracket ']' Key.
  s3eKeySemicolon, //108,    //!< Semicolon ';' Key.
  s3eKeyApostrophe, //109,   //!< Apostrophe (') Key.
  s3eKeyHash, //76,        //!< # Key.
  s3eKeyMinus, //113,        //!< Minus '-' key on main keyboard.
  s3eKeyEquals, //112,       //!< Equals '=' Key.
  s3eKeyComma, //104,        //!< Comma ',' Key.
  s3eKeyPeriod, //105,       //!< Period '.' Key.
  s3eKeySlash, //106,        //!< Forward slash '/' Key.
  s3eKeyBackSlash, //107,    //!< Back slash '\' Key.

  s3eKeyBack                 // back key on Android / WP8 / Kindle etc
};

//==============================================================================
KeyCode TranslateKeyCodeNative(int kc)
{
  const int*  iFind(std::find(karKeyCodeNative, karKeyCodeNative + kKeyCount, kc));
  return static_cast<KeyCode>(iFind - karKeyCodeNative);
}

} // XR
