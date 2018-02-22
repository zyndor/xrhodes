//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_DEBUG_DETAIL_HPP
#define XR_DEBUG_DETAIL_HPP

namespace XR
{
namespace Debug
{

//==============================================================================
enum class AssertAction
{
  Break,
  Continue,
  Ignore,
  IgnoreChannel,
};

typedef AssertAction(*AssertHandler)(char const* channel, char const* message);

void SetAssertHandler(AssertHandler handler);

//==============================================================================
struct Channel
{
  static void SetEnabled(char const* name, bool state);
  static bool IsEnabled(char const* name);

  Channel(char const* name);

  void Trace(char const* format, ...);
  AssertAction Assert(char const* format, ...);

private:
#if defined(XR_DEBUG) // TODO: move the whole debugd.hpp/cpp into debug.cpp and hide all implementation.
  char const* m_name;
#endif
};

} // Debug
} // XR

#define XR__DEBUG_MESSAGE_LENGTH 1024
#define XR__ASSERT_STATE_INIT static char s_isAssertActive = 1;
#define XR__ASSERT_STATE_CHECK(c) (s_isAssertActive == 1 && XR::Debug::Channel::IsEnabled(c))
#define XR__ASSERT_STATE_DISABLE s_isAssertActive = 0

#endif //XR_DEBUG_DETAIL_HPP
