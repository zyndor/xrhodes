//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <XR/debug.hpp>
#include <XR/platform.hpp>
#include <XR/StringProxy.hpp>
#include <set>
#include <cstdarg>

namespace XR
{
namespace Debug
{

#if defined(XR_DEBUG)
std::set<StringProxy> s_disabledChannels;

AssertAction DefaultAssertHandler(char const* name, char const* message);

AssertHandler s_assertHandler = DefaultAssertHandler;
#endif

//==============================================================================
void SetAssertHandler(AssertHandler handler)
{
#if defined(XR_DEBUG)
  s_assertHandler = handler ? handler : DefaultAssertHandler;
#endif
}

//==============================================================================
AssertAction DefaultAssertHandler(char const* channel, char const* message)
{
#if defined(XR_DEBUG)
  // TODO: platform-specific implementation allowing to continue, ignore or break.
  XR_RAWTRACE(("[%s] %s", channel, message));
  return AssertAction::Break;
#else
  return AssertAction::Continue;
#endif
}

//==============================================================================
void Channel::SetEnabled(char const* name, bool state)
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
bool Channel::IsEnabled(char const* name)
{
#if defined(XR_DEBUG)
  return s_disabledChannels.find(name) == s_disabledChannels.end();
#else
  return false;
#endif
}

//==============================================================================
Channel::Channel(char const* name)
: m_name(name)
{}

void Channel::Trace(char const* format, ...)
{
#if defined(XR_DEBUG)
  va_list vl;
  va_start(vl, format);

  char message[XR__DEBUG_MESSAGE_LENGTH];
  vsnprintf(message, sizeof(message), format, vl);
  va_end(vl);

  XR_RAWTRACE(("[%s] %s", m_name, message));
#endif
}

//==============================================================================
AssertAction Channel::Assert(char const* format, ...)
{
#if defined(XR_DEBUG)
  va_list vl;
  va_start(vl, format);

  char message[XR__DEBUG_MESSAGE_LENGTH];
  vsnprintf(message, sizeof(message), format, vl);
  va_end(vl);

  return (*s_assertHandler)(m_name, message);
#else
  return Action::Continue;
#endif
}

} // Debug
} // XR