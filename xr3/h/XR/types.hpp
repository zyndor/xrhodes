//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_TYPES_HPP
#define XR_TYPES_HPP

#include <cstdio>
#include <limits>

//==============================================================================
#if defined XR_SDL
#include <SDL.h>

#if !defined NDEBUG || defined XR_DEBUG_PERFORMANCE
#define XR_DEBUG
#endif  // NDEBUG

#define XR_TRACE(chnl, msg)            printf msg; printf("\n") // stub
#define XR_ASSERT(chnl, cond)          SDL_assert(cond)

#if defined XR_DEBUG
#define XR_ASSERTMSG(chnl, cond, msg)\
  if (!(cond))\
  {\
    XR_TRACE(cnl, msg);\
    SDL_assert(cond);\
  } // stub
#else
#define XR_ASSERTMSG(chnl, cond, msg)\
  SDL_assert(cond) // stub
#endif

#define XR_ERROR(msg)                  void(0); // stub

#elif defined XR_MARMALADE
#include "IwDebug.h"

#if defined IW_DEBUG || defined XR_DEBUG_PERFORMANCE
#define XR_DEBUG
#endif

#define XR_TRACE(chnl, msg)            IwTrace(chnl, msg)
#define XR_ASSERT(chnl, cond)          IwAssert(chnl, cond)
#define XR_ASSERTMSG(chnl, cond, msg)  IwAssertMsg(chnl, cond, msg)

#define XR_ERROR(msg)                  IwError(msg);

#elif defined XR_PS_VITA
#include <scetypes.h>

#define XR_ASSERT(chnl, cond)      assert(cond)
#define XR_ASSERTMSG(chnl, cond, msg) assert(cond, msg)

#endif  //platform

//==============================================================================
#define XR_RAWTRACE(format)	printf format; fflush(stdout);

//==============================================================================
#define XR_CRASHTEST(call)  { printf("At: %s\n", #call, __FILE__, __LINE__); fflush(stdout); call; printf("OK.\n"; }
 
namespace XR
{

//==============================================================================
int16_t ClipToInt16(int32_t val);

//==============================================================================
// implementation
//==============================================================================
inline
int16_t ClipToInt16(int32_t val)
{
  if (((val - std::numeric_limits<int16_t>::min()) &
    ~std::numeric_limits<uint16_t>::max()) != 0)
  {
    bool  over(val > std::numeric_limits<int16_t>::max());
    bool  under(val < std::numeric_limits<int16_t>::min());
    XR_ASSERT(ClipToInt16, over ^ under);
    val = over * std::numeric_limits<int16_t>::max() + under *
      std::numeric_limits<int16_t>::min();
  }

  return static_cast<int16_t>(val);
}

} // XR

#endif // XR_TYPES_HPP