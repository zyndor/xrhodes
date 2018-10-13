//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/debug.hpp"
#include "xr/platform.hpp"
#include "xr/strings/StringProxy.hpp"
#include <set>
#include <cstdarg>

namespace xr
{

#if defined(XR_DEBUG)
namespace
{

void DefaultTraceHandler(char const* channel, char const* message)
{
#if defined(XR_DEBUG)
  XR_RAWTRACE(("[%s] %s\n", channel, message));
#endif
}

AssertAction DefaultAssertHandler(char const* channel, char const* message)
{
#if defined(XR_DEBUG)
  // TODO: platform-specific implementation allowing to continue, ignore or break.
  DefaultTraceHandler(channel, message);
  return AssertAction::Break;
#else
  return AssertAction::Continue;
#endif
}

const size_t kMessageLength = 1024;

std::set<StringProxy> s_disabledChannels;

TraceHandler s_traceHandler = DefaultTraceHandler;
AssertHandler s_assertHandler = DefaultAssertHandler;

} // nonamespace
#endif

//==============================================================================
void SetTraceHandler(TraceHandler handler)
{
#if defined(XR_DEBUG)
  s_traceHandler = handler ? handler : DefaultTraceHandler;
#endif
}

//==============================================================================
void SetAssertHandler(AssertHandler handler)
{
#if defined(XR_DEBUG)
  s_assertHandler = handler ? handler : DefaultAssertHandler;
#endif
}

namespace detail
{

//==============================================================================
void DebugChannel::SetEnabled(char const* name, bool state)
{
#if defined(XR_DEBUG)
  if (state)
  {
    s_disabledChannels.erase(name);
  }
  else
  {
    s_disabledChannels.insert(name);
  }
#endif
}

//==============================================================================
bool DebugChannel::IsEnabled(char const* name)
{
#if defined(XR_DEBUG)
  return s_disabledChannels.find(name) == s_disabledChannels.end();
#else
  return false;
#endif
}

//==============================================================================
DebugChannel::DebugChannel(char const* name)
#if defined(XR_DEBUG)
: m_name(name)
#endif
{}

void DebugChannel::Trace(char const* format, ...)
{
#if defined(XR_DEBUG)
  va_list vl;
  va_start(vl, format);

  char message[kMessageLength];
  vsnprintf(message, sizeof(message), format, vl);
  va_end(vl);

  (*s_traceHandler)(m_name, message);
#endif
}

//==============================================================================
AssertAction DebugChannel::Assert(char const* format, ...)
{
#if defined(XR_DEBUG)
  va_list vl;
  va_start(vl, format);

  char message[kMessageLength];
  vsnprintf(message, sizeof(message), format, vl);
  va_end(vl);

  return (*s_assertHandler)(m_name, message);
#else
  return AssertAction::Continue;
#endif
}

} // detail
} // xr
