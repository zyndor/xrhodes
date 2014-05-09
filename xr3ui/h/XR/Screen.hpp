//
// Nuclear Heart Games
// XRhodes
//
// Screen.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    07/05/2014
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#ifndef XR_SCREEN_HPP
#define XR_SCREEN_HPP

#include <XR/Tweener.hpp>
#include "UIContainer.hpp"
#include "UIEventDispatcher.hpp"
#include "UIRenderer.hpp"

namespace XR
{

class ScreenManager;

//==============================================================================
class Screen
{
public:
  // types
  enum  State
  {
    S_SHOWING,
    S_ACTIVE,
    S_HIDING,
    S_HIDDEN
  };
  
  // structor
  Screen();
  virtual ~Screen();

  // general
  State   GetState() const;
  bool    IsRegistered() const;
  
  Screen* GetPrevious() const;
  void    SetPrevious(Screen* pScreen);
  
  void    Show(ScreenManager& sm, uint32 ms);
  void    Register();
  void    Unregister();
  void    Hide(uint32 ms);
  
  void    Update(uint32 ms);
  
protected:
  // data
  ScreenManager*  m_pScreenManager;  // no ownership
  
  // virtual
  virtual void  _AddElements() =0;
  virtual void  _Show(uint32 ms) =0;
  virtual void  _Hide(uint32 ms) =0;
  virtual void  _RemoveElements() =0;
  
  virtual void  _Register() =0;
  virtual void  _Unregister() =0;
  
private:
  // data
  State           m_state;
  uint32          m_timer;
  bool            m_isRegistered;
  
  Screen*         m_pPrevious;
  
  // internal
  void  _MakeActive();
  void  _MakeHidden();
};

//==============================================================================
// implementation
//==============================================================================
inline
Screen::State Screen::GetState() const
{
  return m_state;
}

//==============================================================================
inline
bool  Screen::IsRegistered() const
{
  return m_isRegistered;
}

//==============================================================================
inline
Screen* Screen::GetPrevious() const
{
  return m_pPrevious;
}

} // XR

#endif // XR_SCREEN_HPP
