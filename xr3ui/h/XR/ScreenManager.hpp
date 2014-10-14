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
  void                Init(int numSprites);

  UIContainer&        GetContainer();
  UIEventDispatcher&  GetDispatcher();
  
  void                Change(Screen* pScreen, int32 delayMs);
  void                Push(Screen* pScreen, int32 delayMs);
  void                Pop(int32 delayMs);
  
  void                Update(int32 ms);
  void                Render();

  void                Shutdown();
  
protected:
  // types
  typedef std::list<Screen*>  ScreenList;

  // static
  ScreenList          m_stack; // no ownership
  Screen*             m_pPrevious;  // no ownership
  
  UIContainer         m_container;
  UIEventDispatcher   m_dispatcher;
  
  UIRenderer          m_renderer;

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
UIEventDispatcher&  ScreenManager::GetDispatcher()
{
  return m_dispatcher;
}

} // XR

#endif // XR_SCREENMANAGER_HPP
