//
// Nuclear Heart Games
// XRhodes
//
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
//
// copyright (c) 2011 - 2016. All rights reserved.
//
//==============================================================================
#ifndef XR_SCREENMANAGER_HPP
#define XR_SCREENMANAGER_HPP

#include <XR/HardStack.hpp>
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
  
  void                Change(Screen& screen, int32 delayMs);
  void                Push(Screen& screen, int32 delayMs);
  void                Pop(int32 delayMs);
  
  void                Update(int32 ms);

  void                Render();
  void                Render(XR::UIRenderer& r);

  void                Shutdown();
  
protected:
  // types
  typedef std::list<Screen*>  ScreenList;

  // static
  ScreenList          m_stack; // no ownership
  Screen*             m_pPrevious;  // no ownership
  
  UIContainer         m_container;
  UIEventNotifier   m_dispatcher;
  
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
  return m_dispatcher;
}

} // XR

#endif // XR_SCREENMANAGER_HPP
