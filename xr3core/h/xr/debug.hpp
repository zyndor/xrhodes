#ifndef XR_DEBUG_HPP
#define XR_DEBUG_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "detail/debugd.hpp"
#include "platform.hpp"
#include <cstdio>

//==============================================================================
///@brief Printf followed by explicit flush.
#define XR_RAWTRACE(format)	printf format; fflush(stdout);

//==============================================================================
///@brief Binary debugging aid - wrap call to help determine whether it causes a
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
#define XR_TRAP __asm__ ("int $3");
#endif
#else
#define XR_TRAP
#endif

//==============================================================================
#define XR_TRACE(chnl, msg) XR_DEBUG_ONLY(if(xr::Debug::Channel::IsEnabled(#chnl)){ xr::Debug::Channel(#chnl).Trace msg; })

//==============================================================================
#define XR_TRACEIF(chnl, cond, msg) XR_DEBUG_ONLY(if((cond) && xr::Debug::Channel::IsEnabled(#chnl)){ xr::Debug::Channel(#chnl).Trace msg; })

//==============================================================================
#define XR_ASSERTMSG(chnl, cond, msg)\
  XR_DEBUG_ONLY({ XR__ASSERT_STATE_INIT if (!(cond) && XR__ASSERT_STATE_CHECK(#chnl)){\
      switch(xr::Debug::Channel(#chnl).Assert msg) {\
        case xr::Debug::AssertAction::Break: XR_TRAP break;\
        case xr::Debug::AssertAction::Continue: break;\
        case xr::Debug::AssertAction::Ignore: XR__ASSERT_STATE_DISABLE; break;\
        case xr::Debug::AssertAction::IgnoreChannel: xr::Debug::Channel::SetEnabled(#chnl, false); break;\
    }}})

//==============================================================================
#define XR_ASSERT(chnl, cond) XR_ASSERTMSG(chnl, cond, ("Assertion failed: %s (%s:%d)", #cond, __FILE__, __LINE__))

//==============================================================================
#define XR_ERROR(msg)\
  {\
    XR_RAWTRACE(msg);\
    XR_TRAP\
  }\

#endif  //XR_DEBUG_HPP