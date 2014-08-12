//
// Nuclear Heart Games
// XRhodes
//
// State.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    29/11/2012
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_STATE_HPP
#define XR_STATE_HPP

#include <list>
#include "HardStack.hpp"
#include "Module.hpp"

namespace XR
{

//==============================================================================
///@brief An application state. Lifecycle:
///@ Init() when pushed onto the stack
///@   Enter(), FadeIn() when previous state has exited.
///@     UpdateFadeIn() when fading in
///@     Update() when active (fadeout has finished)
///@   FadeOut() when new state was requested
///@     UpdateFadeOut() while fading out
///@   Exit() when fadeout has finished
///@ Shutdown() when popped from stack
class State: protected Module
{
public:
  ///@brief Manages the Update() and Render()ing of states as well as state
  /// transitions
  class Manager
  {
    XR_NONCOPY_DECL(Manager)

  public:
    // structors
    Manager();
    ~Manager();

    // general
    int32 GetFadeTimer() const;

    void  Push(State* pState, int tFade = 0); // no ownership transfer
    void  Pop(int tFade = 0); // no ownership transfer
    void  Change(State* pState, int tFade = 0); // no ownership transfer

    void  Update(int32 tDelta);
    void  Render();

    void  Clear();
    
  private:
    // types
    typedef std::list<State*> StateList;
    
    // data
    StateList m_stack;
    
    State*  m_pExiting;  // no ownership
    State*  m_pCurrent;  // no ownership, processed if there's no exiting state.

    bool    m_doShutdown;
    int     m_fadeDelay;
    int     m_tFade;

    // internal
    void  _QuickExit();
  };

  // using
  using Module::Render;

  // structors
  State();
  virtual ~State();
  
  // virtual
  virtual void  Enter() =0; // when requested state becomes active
  virtual void  Exit() =0;  // when requested state becomes inactive; must reset changes made since Enter()

  virtual void  Update(int32 tDelta) =0;

  // general
  virtual void  FadeIn(int32 tFade);
  virtual void  UpdateFadeIn(int32 tDelta);
  virtual void  FadeOut(int32 tFade);
  virtual void  UpdateFadeOut(int32 tDelta);

  // friends
  friend class  Manager;

protected:
  // virtual
  virtual void  Init() =0;  // when state gets pushed on stack
  virtual void  Shutdown() =0;  // when state gets popped from stack; must reset changes since Init()
};

//==============================================================================
// implementation
//==============================================================================
inline
int32 State::Manager::GetFadeTimer() const
{
  return m_tFade;
}

} // XR

#endif // XR_STATE_HPP
