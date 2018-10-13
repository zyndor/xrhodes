#ifndef XR_SCREEN_HPP
#define XR_SCREEN_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include <cstdint>

namespace xr
{

class ScreenManager;

//==============================================================================
///@brief A configuration of UIElements which may be shown or hidden using a
/// ScreenManager. Screens go through four states: showing, active, hiding, and
/// hidden. Events are only handled in the 'active' state. Elements are only
/// visible between showing and hiding. Showing and hiding allows an animation
/// for a given duration, specified when showing or hiding a scene (which are
/// performed at the respective hiding or showing of another scene).
class Screen
{
public:
  // types
  enum class State
  {
    Showing,
    Active,
    Hiding,
    Hidden
  };

  using BecomeActiveCallback = void(*)(Screen* screen, void* userData);

  // structors
  Screen();
  virtual ~Screen();

  // virtual
  ///@brief Provides an opportunity to reposition the UIElements of the Screen,
  /// based on the given screen @a width and @a height.
  virtual void    Reposition(int width, int height) =0;

  // general
  ///@brief The current state of the Screen.
  State GetState() const;

  ///@brief Whether event listener registration has been performed.
  ///@note This is done when the Screen enters the Active state, however can also
  /// be done manually.
  bool  IsRegistered() const;

  ///@brief Get the ScreenManager that this Screen is shown on, if any.
  ///@note This is reset to nullptr once the Screen enters the Hidden state.
  ScreenManager*  GetManager() const;  // no ownership transfer

  ///@brief Sets the function that will be called when this Screen enters the
  /// Active state.
  void  SetOnBecomeActive(BecomeActiveCallback becomeActive, void* userData);

  ///@brief Shows the screen on the given ScreenManager @a sm, with a delay of
  /// @a ms milliseconds, which may be used for an animation.
  ///@note The Screen must be in one of the Hiding or Hidden states.
  void  Show(ScreenManager& sm, int32_t ms);

  ///@brief Performs the registration of selected elements as mouse event listeners.
  void  Register();

  ///@brief Performs the de-registration of mouse event listeners.
  void  Unregister();

  ///@brief Hides the screen with a delay of @a ms milliseconds, which may be used
  /// for an animation.
  void  Hide(uint32_t ms);

  //@brief Updates showing / hiding animations and handles state transitions.
  void  Update(uint32_t ms);

protected:
  // data
  ScreenManager*  m_manager;  // no ownership

  // virtual
  ///@brief Adds elements to the container of the cached ScreenManager, making
  /// them visible.
  virtual void    AddElements() =0;

  ///@brief Performs an animation of @a ms milliseconds, to make the UIElements of the
  /// screen, shown.
  virtual void    ShowImpl(uint32_t ms) =0;

  ///@brief Performs an animation of @a ms milliseconds, to make the UIElements of the
  /// screen, hidden.
  virtual void    HideImpl(uint32_t ms) =0;

  ///@brief Removes the elements from the container of the cached ScreenManager,
  /// effectively hiding them from the screen.
  virtual void    RemoveElements() =0;

  ///@brief Registers elements that need to listen to mouse events, on the UIEventNotifier
  /// of the cached ScreenManager.
  virtual void    RegisterImpl() =0;

  ///@brief Unregisters listeners from the UIEventNotifier of the cached ScreenManager.
  virtual void    UnregisterImpl() =0;

private:
  // data
  State           m_state;
  uint32_t        m_timer : 31;
  bool            m_isRegistered : 1;

  BecomeActiveCallback  m_onBecomeActive;
  void*                 m_onBecomeActiveData;

  // internal
  void            MakeActive();
  void            MakeHidden();
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

} // xr

#endif // XR_SCREEN_HPP
