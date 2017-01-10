//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_JSON_HPP
#define XR_JSON_HPP

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
  kNewLine = '\n',
  kDecimal = '.',
  kExponential = 'e'
};

//==============================================================================
extern const char*  kNull;
extern const char*  kTrue;
extern const char*  kFalse;

const int kMaxParseDepthDefault = 32;

} // JSON

} // XR

#endif // XR_JSON_HPP
