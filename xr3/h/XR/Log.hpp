//
// Nuclear Heart Games
// XRhodes
//
// Log.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    22/10/2013
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_LOG_HPP
#define XR_LOG_HPP

#include "types.hpp"

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