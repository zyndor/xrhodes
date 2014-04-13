//
// Nuclear Heart Games
// XRhodes
//
// json.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    05/01/2014
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_JSON_HPP
#define XR_JSON_HPP

#include "types.hpp"

namespace XR {

namespace JSON  {

//==============================================================================
enum  Char
{
  kQuot = '\"',
  kColon = ':',
  kComma = ',',
  kObjectBegin = '{',
  kObjectEnd = '}',
  kArrayBegin = '[',
  kArrayEnd = ']',
  kNewLine = '\n'
};

//==============================================================================
extern const char*  kNull;
extern const char*  kTrue;
extern const char*  kFalse;

const int    kMaxParseDepthDefault = 32;

} // JSON

} // XR

#endif // XR_JSON_HPP
