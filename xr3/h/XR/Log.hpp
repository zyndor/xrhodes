//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_LOG_HPP
#define XR_LOG_HPP

#include "fundamentals.hpp"

namespace XR
{

//==============================================================================
class Log
{
  XR_NONOBJECT_DECL(Log)

public:
  static void Init();
  static void Exit();
  
  static void Write(const char* pString);
};

} // XR

#endif // XR_LOG_HPP