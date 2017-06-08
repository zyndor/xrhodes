//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_DEBUG_HPP
#define XR_DEBUG_HPP

#include "platform.hpp"
#include <cstdio>

//==============================================================================
///@brief Printf followed by explicit flush.
#define XR_RAWTRACE(format)	printf format; fflush(stdout);

//==============================================================================
///@brief Binary debugging aid - wrap call to ascertain whether it causes a
/// crash / exception.
#define XR_SURVIVE(call)  { XR_RAWTRACE(("SURVIVE: %s (%s:%d)\n", #call, __FILE__, __LINE__)); call; printf("OK.\n"); }

//==============================================================================
#if !defined(XR_DEBUG) && defined(XR_DEBUG_PERFORMANCE)
#define XR_DEBUG
#endif

//==============================================================================
#if defined(XR_DEBUG)
#define XR_DEBUG_ONLY(op) op
#else
#define XR_DEBUG_ONLY(op)
#endif

//==============================================================================
#if defined(XR_DEBUG)
#if defined(XR_COMPILER_MSVC)
#define XR_TRAP __debugbreak();
#elif defined(XR_CPU_ARM)
#define XR_TRAP __builtin_trap();
#else
#define XR_TRAP { int* const trap = (int*)3L; *trap = 3; }
#endif
#else
#define XR_TRAP
#endif

//==============================================================================
#define XR__TRACE_IMPL(chnl, msg)\
  printf("[%s]", #chnl);\
  printf msg;\
  XR_RAWTRACE(("\n"))  // IMPROVE

//==============================================================================
#define XR_TRACE(chnl, msg)\
  XR_DEBUG_ONLY(XR__TRACE_IMPL(chnl, msg)) // IMPROVE

//==============================================================================
#define XR_ASSERTMSG(chnl, cond, msg)\
  XR_DEBUG_ONLY(if (!(cond)) {\
      XR__TRACE_IMPL(chnl, msg); XR_TRAP\
    }) // IMPROVE

//==============================================================================
#define XR_ASSERT(chnl, cond)\
  XR_DEBUG_ONLY(if (!(cond)) {\
      XR__TRACE_IMPL(chnl, ("Assertion failed: %s (%s:%d)", #cond, __FILE__, __LINE__));\
    }) // IMPROVE

//==============================================================================
#define XR_ERROR(msg)\
  {\
    XR_RAWTRACE(msg);\
    XR_TRAP\
  }\

#endif  //XR_DEBUG_HPP