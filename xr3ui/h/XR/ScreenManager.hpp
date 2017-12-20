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
#include "UIRenderer.hpp"

namespace XR
{

//==============================================================================
class ScreenManager
{
public:
  // structors
  ScreenManager();
  ~ScreenManager();

  // general
  UIContainer&        GetContainer();
  UIEventNotifier&    GetNotifier();

  void                Change(Screen& screen, int32_t delayMs);
  void                Push(Screen& screen, int32_t delayMs);
  void                Pop(int32_t delayMs);

  void                Update(int32_t ms);

  void                Render(IUIRenderer& r);

  void                Shutdown();

protected:
  // types
  typedef std::list<Screen*>  ScreenList;

  // static
  ScreenList          m_stack; // no ownership
  Screen*             m_pPrevious;  // no ownership

  UIContainer         m_container;
  UIEventNotifier     m_notifier;

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

} // XR

#endif // XR_SCREENMANAGER_HPP
