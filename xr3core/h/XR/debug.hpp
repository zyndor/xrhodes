//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_DEBUG_HPP
#define XR_DEBUG_HPP

#include <cstdio>

//==============================================================================
///@brief Printf followed by explicit flush.
#define XR_RAWTRACE(format)	printf format; fflush(stdout);

//==============================================================================
///@brief Binary debugging aid - wrap call to ascertain whether it causes a crash.
#define XR_SURVIVE(call)  { XR_RAWTRACE(("SURVIVE: %s (%s:%d)\n", #call, __FILE__, __LINE__)); call; printf("OK.\n"); }

//==============================================================================
// Platform / backend specific trace / assert functionality.
//
#if defined(XR_SDL)
// SDL -- for now, use printf and SDL_assert.
#include <SDL_assert.h>

#if !defined(XR_DEBUG) && defined(XR_DEBUG_PERFORMANCE)
#define XR_DEBUG
#endif

#if defined(XR_DEBUG)
#define XR_TRACE(chnl, msg)\
  printf("[%s]", #chnl); printf msg; XR_RAWTRACE(("\n")); // IMPROVE
#else
#define XR_TRACE(chnl, msg)
#endif

#define XR_ASSERT(chnl, cond)         SDL_assert(cond)
#define XR_ASSERTMSG(chnl, cond, msg) SDL_assert(cond) // FIXME: chnl, msg use

#define XR_ERROR(msg)\
  {\
    XR_RAWTRACE(msg);\
    SDL_TriggerBreakpoint();\
  }\

//
#elif defined(XR_MARMALADE)
// Marmalade -- just use Marmalade's trace / assert functionality
#include "IwDebug.h"

#if !defined(XR_DEBUG) && (defined(IW_DEBUG) || defined(XR_DEBUG_PERFORMANCE))
#define XR_DEBUG
#endif

#define XR_TRACE(chnl, msg)             IwTrace(chnl, msg)
#define XR_ASSERT(chnl, cond)           IwAssert(chnl, cond)
#define XR_ASSERTMSG(chnl, cond, msg)   IwAssertMsg(chnl, cond, msg)
#define XR_ERROR(msg)                   IwError(msg)

#endif

#endif  //XR_DEBUG_HPP