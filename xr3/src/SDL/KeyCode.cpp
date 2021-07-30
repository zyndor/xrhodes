//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/KeyCode.hpp"
#include "SDL2/SDL_scancode.h"
#include <algorithm>
#include <array>

namespace xr
{
namespace
{

//==============================================================================
struct KeyMapping
{
  uint32_t native;
  KeyCode xr;
};

bool operator<(KeyMapping const& km, uint32_t sc)
{
  return km.native < sc;
}

const KeyMapping kKeyMappings[] =
{
#define KEYMAP(x) { SDL_SCANCODE_##x, K_##x }
#define KEYMAP2(x, y) { SDL_SCANCODE_##x, K_##y }

  KEYMAP(A) /*4*/,
  KEYMAP(B) /*5*/,
  KEYMAP(C) /*6*/,
  KEYMAP(D) /*7*/,
  KEYMAP(E) /*8*/,
  KEYMAP(F) /*9*/,
  KEYMAP(G) /*10*/,
  KEYMAP(H) /*11*/,
  KEYMAP(I) /*12*/,
  KEYMAP(J) /*13*/,
  KEYMAP(K) /*14*/,
  KEYMAP(L) /*15*/,
  KEYMAP(M) /*16*/,
  KEYMAP(N) /*17*/,
  KEYMAP(O) /*18*/,
  KEYMAP(P) /*19*/,
  KEYMAP(Q) /*20*/,
  KEYMAP(R) /*21*/,
  KEYMAP(S) /*22*/,
  KEYMAP(T) /*23*/,
  KEYMAP(U) /*24*/,
  KEYMAP(V) /*25*/,
  KEYMAP(W) /*26*/,
  KEYMAP(X) /*27*/,
  KEYMAP(Y) /*28*/,
  KEYMAP(Z) /*29*/,

  KEYMAP(1) /*30*/,
  KEYMAP(2) /*31*/,
  KEYMAP(3) /*32*/,
  KEYMAP(4) /*33*/,
  KEYMAP(5) /*34*/,
  KEYMAP(6) /*35*/,
  KEYMAP(7) /*36*/,
  KEYMAP(8) /*37*/,
  KEYMAP(9) /*38*/,
  KEYMAP(0) /*39*/,

  KEYMAP2(RETURN, ENTER) /*40*/,
  KEYMAP(ESCAPE) /*41*/,
  KEYMAP(BACKSPACE) /*42*/,
  KEYMAP(TAB) /*43*/,
  KEYMAP(SPACE) /*44*/,

  KEYMAP(MINUS) /*45*/,
  KEYMAP(EQUALS) /*46*/,
  KEYMAP2(LEFTBRACKET, LBRACKET) /*47*/,
  KEYMAP2(RIGHTBRACKET, RBRACKET) /*48*/,
  KEYMAP(BACKSLASH) /*49*/,
  KEYMAP2(NONUSHASH, HASH), /*50*/

  KEYMAP(SEMICOLON) /*51*/,
  KEYMAP(APOSTROPHE) /*52*/,
  KEYMAP2(GRAVE, BACKTICK) /*53*/,
  KEYMAP(COMMA) /*54*/,
  KEYMAP(PERIOD) /*55*/,
  KEYMAP(SLASH) /*56*/,

  KEYMAP(CAPSLOCK) /*57*/,
  KEYMAP(F1) /*58*/,
  KEYMAP(F2) /*59*/,
  KEYMAP(F3) /*60*/,
  KEYMAP(F4) /*61*/,
  KEYMAP(F5) /*62*/,
  KEYMAP(F6) /*63*/,
  KEYMAP(F7) /*64*/,
  KEYMAP(F8) /*65*/,
  KEYMAP(F9) /*66*/,
  KEYMAP(F10) /*67*/,
  KEYMAP(F11) /*68*/,
  KEYMAP(F12) /*69*/,

  KEYMAP(PAUSE) /*72*/,

  KEYMAP(INSERT) /*73*/,
  KEYMAP(HOME) /*74*/,
  KEYMAP2(PAGEUP, PGUP) /*75*/,
  KEYMAP(DELETE) /*76*/,
  KEYMAP(END) /*77*/,
  KEYMAP2(PAGEDOWN, PGDOWN) /*78*/,

  KEYMAP(RIGHT) /*79*/,
  KEYMAP(LEFT) /*80*/,
  KEYMAP(DOWN) /*81*/,
  KEYMAP(UP) /*82*/,

  KEYMAP(MENU) /*118*/,
  KEYMAP2(SELECT, OK) /*119 ?? */,

  KEYMAP(LCTRL) /*224*/,
  KEYMAP(LSHIFT) /*225*/,
  KEYMAP(LALT) /*226*/, /**< alt, option */
  KEYMAP(RCTRL) /*228*/,
  KEYMAP(RSHIFT) /*229*/,
  KEYMAP(RALT) /*230*/, /**< alt gr, option */

  KEYMAP2(AC_BACK, BACK), /*270*/

#undef KEYMAP
#undef KEYMAP2
};

}

//==============================================================================
const int kKeyCodesNative[] =
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

  SDL_SCANCODE_INSERT, //73,
  SDL_SCANCODE_HOME, //74,
  SDL_SCANCODE_PAGEUP, //75,
  SDL_SCANCODE_PAGEDOWN, //78,
  SDL_SCANCODE_END, //77,
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

  SDL_SCANCODE_AC_BACK, // 270
  SDL_SCANCODE_MENU,  // 118
  SDL_SCANCODE_SELECT // 119 -- might not
};

//==============================================================================
KeyCode TranslateKeyCodeNative(uint32_t kc)
{
  auto iFind = std::lower_bound(std::begin(kKeyMappings), std::end(kKeyMappings), kc);
  return (iFind != std::end(kKeyMappings) && iFind->native == kc) ? iFind->xr : K_UNKNOWN;
}

} // xr
