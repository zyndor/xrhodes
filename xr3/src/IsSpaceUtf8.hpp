#ifndef XR_ISSPACEUTF8_HPP
#define XR_ISSPACEUTF8_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include <cstdint>

namespace xr
{

//==============================================================================
///@return Whether the code point @a cp denotes a white space.
// Reason: https://stackoverflow.com/a/48282063
// Source:
//   https://en.wikipedia.org/wiki/Whitespace_character
//   http://jkorpela.fi/chars/spaces.html
inline
bool IsSpaceUtf8(uint32_t cp)
{
  switch (cp)
  {
  case 0x0009: // TAB
  case 0x000A: // LINE FEED
  case 0x000B: // LINE TAB
  case 0x000C: // FORM FEED
  case 0x000D: // CARRIAGE RETURN
  case 0x0020: // SPACE
  case 0x0085: // NEXT LINE
  case 0x00A0: // NO-BREAK SPACE
  case 0x1680: // OGHAM SPACE MARK
  case 0x180E: // MONGOLIAN VOWEL SEPARATOR
  case 0x2000: // EN QUAD
  case 0x2001: // EM QUAD
  case 0x2002: // EN SPACE (nut)
  case 0x2003: // EM SPACE (mutton)
  case 0x2004: // THREE-PER-EM SPACE
  case 0x2005: // FOUR-PER-EM SPACE
  case 0x2006: // SIX-PER-EM SPACE
  case 0x2007: // FIGURE SPACE
  case 0x2008: // PUNCTUATION SPACE
  case 0x2009: // THIN SPACE
  case 0x200A: // HAIR SPACE
  case 0x200B: // ZERO WIDTH SPACE
  case 0x202C: // ZERO WIDTH NON-JOINER
  case 0x202D: // ZERO WIDTH JOINER
  case 0x2028: // LINE SEPARATOR
  case 0x2029: // PARAGRAPH SEPARATOR
  case 0x202F: // NARROW NO-BREAK SPACE
  case 0x205F: // MEDIUM MATHEMATICAL SPACE
  case 0x2060: // WORD JOINER
  case 0x3000: // IDEOGRAPHIC SPACE
  case 0xFEFF: // ZERO WIDTH NO-BREAK SPACE
    return true;
          
  default:
    return false;
  }
}

}

#endif //XR_ISPACEUTF8_HPP
