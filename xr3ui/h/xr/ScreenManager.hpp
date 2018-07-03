#ifndef XR_SCREENMANAGER_HPP
#define XR_SCREENMANAGER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "Screen.hpp"
#include "UIContainer.hpp"
#include "UIRenderer.hpp"
#include "UIEventNotifier.hpp"

namespace xr
{

//==============================================================================
///@brief Manages the lifecycle of Screens, offering a stack of Screenes to get
/// added to and popped off from, and coordinating the cross-fading (i.e.
/// Hide() and Show()) of Screens.
class ScreenManager
{
public:
  // structors
  ScreenManager();
  ~ScreenManager();

  // general
  ///@return The UIContainer that screens get added to.
  UIContainer&  GetContainer();

  ///@return An event notifier which screens can register their listener elements
  /// against.
  UIEventNotifier&  GetNotifier();

  ///@brief Hides the current screen, and replaces it on the top of the stack
  /// with @a screen, in @a delayMs milliseconds.
  ///@note If a hiding animation was already in progress, it will be fast forwarded
  /// to completion.
  void  Change(Screen& screen, int32_t delayMs);

  ///@brief Pushes @a screen on the stack, on top of the current one, in @a delayMs
  /// milliseconds.
  ///@note If a hiding animation was already in progress, it will be fast forwarded
  /// to completion.
  void  Push(Screen& screen, int32_t delayMs);

  ///@brief Hides the current screen, removes it from the top of the stack
  /// and replacing it with the one below (if any), in @a delayMs milliseconds.
  ///@note If a hiding animation was already in progress, it will be fast forwarded
  /// to completion.
  void  Pop(int32_t delayMs);

  ///@brief Updates hiding / showing animations and handles state transitions.
  void  Update(int32_t ms);

  ///@brief Performs rendering of all screens on the stack.
  void  Render(IUIRenderer& r);

  ///@brief Removes all screens from the stack, hiding them as needed (using a
  /// 0 milliseconds delay).
  void  Shutdown();

protected:
  // types
  typedef std::list<Screen*>  ScreenList;

  // static
  ScreenList  m_stack; // no ownership
  Screen* m_previous;  // no ownership

  UIContainer m_container;
  UIEventNotifier m_notifier;

  // internal
  void  _ClearExiting();
};

//==============================================================================
// implementation
//==============================================================================
inline
UIContainer&        ScreenManager::GetContainer()
{
  return m_container;
}

//==============================================================================
inline
UIEventNotifier&  ScreenManager::GetNotifier()
{
  return m_notifier;
}

} // xr

#endif // XR_SCREENMANAGER_HPP
