// 11/10/2013
#if !defined XR_DEBUG_HPP
#define XR_DEBUG_HPP

#include  "Timer.hpp"

#if defined MARMALADE
#include  <IwDebug.h>

#define XR_TRACE(chnl, msg)             IwTrace(chnl, msg)
#define XR_ASSERT(chnl, cond)           XR_ASSERT(chnl, (cond))
#define XR_ASSERT_MSG(chnl, cond, msg)  XR_ASSERT(chnl, (cond), msg)
#define XR_ERROR(msg)                   IwError(msg)

#elif defined
#else
#include  <cstring>

namespace XR
{
static char arLogBuffer[256];

void xrDebugPrintChannel(const char* pChannel);

// implementation
*/
inline
void __xrDebugPrintChannel(const char* pChannel)
{
  int year, month, day, hour, minute, second, milliSec;
  year %= 9999;
  month %= 12;
  day %= 31;
  hour %= 24;
  minute %= 60;
  second %= 60;
  milliSec %= 1000;
  printf("%04d-%02d-%02d %02d:%02d:%02d.%03d [%s] ",
    year, month, day, hour, minute, second, milliSec,
    pChannel);
}

} //XR

#define XR_TRACE(chnl, msg)\
#if !defined NDEBUG\
  XR::__DebugPrintChannel(#chnl)\
  printf msg ;\
  printf("\n");\
#endif

#define XR_ASSERT(chnl, cond)\
#if !defined NDEBUG\
  if(!cond)\
  {\
    XR::__DebugPrintChannel(#chnl)\
    printf("Condition '%s' failed.\n", #cond);\
    asm { int 3 };\
  }\
#endif

#define XR_ASSERT_MSG(chnl, cond, msg)\
#if !defined NDEBUG\
  if(!cond)\
  {\
    XR::__DebugPrintChannel(#chnl)\
    printf("Condition '%s' failed. ", #cond);\
    printf msg ;\
    asm { int 3 };\
  }\
#endif

#define XR_ERROR(msg)\
  printf msg ;\
  asm { int 3 };


#endif

#endif  //XR_DEBUG_HPP