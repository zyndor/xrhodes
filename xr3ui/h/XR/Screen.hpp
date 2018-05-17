#ifndef XR_SCREEN_HPP
#define XR_SCREEN_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include "UIContainer.hpp"
#include "UIEventNotifier.hpp"
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

  typedef void(*BecomeActiveCallback)(Screen* screen, void* userData);

  // structors
  Screen();
  virtual ~Screen();

  // virtual
  virtual void    Reposition(int width, int height) =0;

  // general
  State           GetState() const;
  bool            IsRegistered() const;
  ScreenManager*  GetManager() const;  // no ownership transfer

  void            SetOnBecomeActive(BecomeActiveCallback becomeActive, void* userData);

  void            Show(ScreenManager& sm, int32_t ms);
  void            Register();
  void            Unregister();
  void            Hide(uint32_t ms);

  void            Update(uint32_t ms);

protected:
  // data
  ScreenManager*  m_manager;  // no ownership

  // virtual
  virtual void    _AddElements() =0;
  virtual void    _Show(uint32_t ms) =0;
  virtual void    _Hide(uint32_t ms) =0;
  virtual void    _RemoveElements() =0;

  virtual void    _Register() =0;
  virtual void    _Unregister() =0;

private:
  // data
  State           m_state;
  uint32_t        m_timer;
  bool            m_isRegistered;

  BecomeActiveCallback  m_onBecomeActive;
  void*                 m_onBecomeActiveData;

  // internal
  void            _MakeActive();
  void            _MakeHidden();
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
ScreenManager*  Screen::GetManager() const
{
  return m_manager;
}

} // XR

#endif // XR_SCREEN_HPP
