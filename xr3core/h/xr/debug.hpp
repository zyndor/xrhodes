#ifndef XR_DEBUG_HPP
#define XR_DEBUG_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "platform.hpp"
#include <cstdio>

//==============================================================================
///@brief Printing to stdout followed by explicit flush.
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
///@brief If the given channel is enabled, it passes the given @a channel and
/// formattable message to the trace handler.
///@note msg must be a parenthesised, C-style message format + parameters.
#define XR_TRACE(channel, msg) XR_DEBUG_ONLY(if(xr::detail::DebugChannel::IsEnabled(#channel)){ xr::detail::DebugChannel(#channel).Trace msg; })

//==============================================================================
///@brief If @a cond evaluates to true and the given channel is enabled, it
/// passes the given @a channel and formattable message to the trace handler.
///@note msg must be a parenthesised, C-style message format + parameters.
#define XR_TRACEIF(channel, cond, msg) XR_DEBUG_ONLY(if((cond) && xr::detail::DebugChannel::IsEnabled(#channel)){ xr::detail::DebugChannel(#channel).Trace msg; })

//==============================================================================
#define XR__ASSERT_STATE_INIT static char xrDebugIsAssertActive = 1;
#define XR__ASSERT_STATE_CHECK(c) (xrDebugIsAssertActive == 1 && xr::detail::DebugChannel::IsEnabled(c))
#define XR__ASSERT_STATE_DISABLE xrDebugIsAssertActive = 0

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

///@brief If the condition evaluates to false and the channel is enabled, it
/// passes the given channel and formattable message to the assert handler.
///@note msg must be a parenthesised, C-style message format + parameters.
#define XR_ASSERTMSG(channel, cond, msg)\
  XR_DEBUG_ONLY({ XR__ASSERT_STATE_INIT if (!(cond) && XR__ASSERT_STATE_CHECK(#channel)){\
      switch(xr::detail::DebugChannel(#channel).Assert msg) {\
        case xr::AssertAction::Break: XR_TRAP break;\
        case xr::AssertAction::Continue: break;\
        case xr::AssertAction::Ignore: XR__ASSERT_STATE_DISABLE; break;\
        case xr::AssertAction::IgnoreChannel: xr::detail::DebugChannel::SetEnabled(#channel, false); break;\
    }}})

//==============================================================================
///@brief If the condition evaluates to false and the channel is enabled, it
/// passes the given channel and default message specifying the location of the
/// assert, to the assert handler.
#define XR_ASSERT(channel, cond) XR_ASSERTMSG(channel, cond, ("Assertion failed: %s (%s:%d)", #cond, __FILE__, __LINE__))

//==============================================================================
#define XR_ERROR(msg)\
  {\
    XR_RAWTRACE(msg);\
    XR_TRAP\
  }\

namespace xr
{

//==============================================================================
enum class AssertAction
{
  Break,
  Continue,
  Ignore,
  IgnoreChannel,
};

using TraceHandler = void(*)(char const* channel, char const* message);
using AssertHandler = AssertAction(*)(char const* channel, char const* message);

///@brief Sets the function that a trace channel and formatted message are passed
/// to, once the channel is checked and found active. Passing nullptr as @a handler
/// will set the default handler, which writes to stdout.
void SetTraceHandler(TraceHandler handler);

///@brief Sets the function which an assert channel and formatted message are
/// passed to, once the channel is checked and found active. Passing nullptr as
/// @a handler will set the default handler.
void SetAssertHandler(AssertHandler handler);

namespace detail
{
//==============================================================================
struct DebugChannel
{
  static void SetEnabled(char const* name, bool state);
  static bool IsEnabled(char const* name);

  DebugChannel(char const* name);

  void Trace(char const* format, ...);
  AssertAction Assert(char const* format, ...);

private:
#if defined(XR_DEBUG)
  char const* m_name;
#endif
};

} // detail
} // XR


#endif  //XR_DEBUG_HPP
