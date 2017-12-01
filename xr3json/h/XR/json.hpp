#ifndef XR_JSON_HPP
#define XR_JSON_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

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
extern const char* const kNull;
extern const char* const kTrue;
extern const char* const kFalse;

const int kMaxParseDepthDefault = 32;

} // JSON

} // XR

#endif // XR_JSON_HPP
