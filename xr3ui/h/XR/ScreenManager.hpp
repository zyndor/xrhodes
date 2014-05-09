//
// Nuclear Heart Games
// XRhodes
//
// ScreenManager.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    07/05/2014
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#ifndef XR_SCREENMANAGER_HPP
#define XR_SCREENMANAGER_HPP

#include  <XR/HardStack.hpp>
#include  "Screen.hpp"
#include  "UIRenderer.hpp"

namespace XR
{

//==============================================================================
class ScreenManager
{
public:
  // types
  
  // structors
  ScreenManager();
  ~ScreenManager();

  // general
  Tweener&            GetTweener();
  UIContainer&        GetContainer();
  UIEventDispatcher&  GetDispatcher();
  
  void Change(Screen* pScreen, uint32 delayMs);
  void Push(Screen* pScreen, uint32 delayMs);
  void Pop(uint32 delayMs);
  
  void Update(int32 ms);
  void Render();
  
protected:
  // static
  Screen*                 m_pCurrent; // no ownership
  Screen*                 m_pLast;  // no ownership
  
  Tweener                 m_tweener;
  UIContainer             m_container;
  UIEventDispatcher       m_dispatcher;
  
  UIRenderer              m_renderer;
};

//==============================================================================
// implementation
//==============================================================================
inline
Tweener&  ScreenManager::GetTweener()
{
  return m_tweener;
}

//==============================================================================
inline
UIContainer&        ScreenManager::GetContainer()
{
  return m_container;
}

//==============================================================================
inline
UIEventDispatcher&  ScreenManager::GetDispatcher()
{
  return m_dispatcher;
}

} // XR

#endif // XR_SCREENMANAGER_HPP
