//
// Nuclear Heart Games
// XRhodes
//
// types.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    11/09/2013
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_TYPES_HPP
#define XR_TYPES_HPP

#include <cstdio>
#include <limits>

//==============================================================================
#if !defined NDEBUG || defined XR_DEBUG_PERFORMANCE
#define XR_DEBUG
#endif  // NDEBUG

//==============================================================================
#if defined XR_SDL
#include <SDL2/SDL.h>

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

typedef Sint8           int8;
typedef Sint16          int16;
typedef Sint32          int32;
typedef Sint64          int64;

typedef Uint8           uint8;
typedef Uint16          uint16;
typedef Uint32          uint32;
typedef Uint64          uint64;

#elif defined XR_MARMALADE
#include "s3eTypes.h"
#include "IwDebug.h"

#define XR_TRACE(chnl, msg)            IwTrace(chnl, msg)
#define XR_ASSERT(chnl, cond)          IwAssert(chnl, cond)
#define XR_ASSERTMSG(chnl, cond, msg)  IwAssertMsg(chnl, cond, msg)

#define XR_ERROR(msg)                  IwError(msg);

#elif defined XR_PS_VITA
#include <scetypes.h>

typedef SceInt8         int8;
typedef SceInt16        int16;
typedef SceInt32        int32;
typedef SceInt64        int64;

typedef SceUInt8        uint8;
typedef SceUInt16       uint16;
typedef SceUInt32       uint32;
typedef SceUInt64       uint64;

#define XR_ASSERT(chnl, cond)      assert(cond)
#define XR_ASSERTMSG(chnl, cond, msg) assert(cond, msg)

#endif  //platform

//==============================================================================
#define XR_ONE_OVER_0XFF  (1.0f / (float)0xff)

//==============================================================================
#define XR_MASK_ID(id) (1 << static_cast<uint32>(id))

//==============================================================================
#define XR_MASK_HAS_ID(mask, id) (((mask) & XR_MASK_ID(id)) != 0)

//==============================================================================
#define XR_NONOBJECT_DECL(className) \
  private:  \
    className();  \
    ~className();

//==============================================================================
#define XR_NONCOPY_DECL(className) \
  private:  \
    className(const className& rhs);\
    className& operator=(const className& rhs);

//==============================================================================
#define XR_ARR_SIZE(a) (sizeof(a) / sizeof((a)[0]))

namespace XR
{

//==============================================================================
template  <typename T>
struct Identity 
{
  typedef T Type;
};

int32 Align(int32 value, int32 alignment);
int16 ClipToInt16(int32 val);

//==============================================================================
// implementation
//==============================================================================
inline
int16 ClipToInt16( int32 val )
{
  if (((val - std::numeric_limits<int16>::min()) &
    ~std::numeric_limits<int16>::max()) != 0)
  {
    bool  over(val > std::numeric_limits<int16>::max());
    bool  under(val < std::numeric_limits<int16>::min());
    XR_ASSERT(ClipToInt16, over ^ under);
    val = (over * std::numeric_limits<int16>::max()) +
      (under * std::numeric_limits<int16>::min());
  }

  return static_cast<int16>(val);
}

//==============================================================================
inline
int32 Align(int32 value, int32 alignment)
{
  XR_ASSERT(Align, alignment > 0);
  return value + alignment - (value % alignment);
}

} // XR

#endif // XR_TYPES_HPP