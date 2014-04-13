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

#include <vector>
#include "HardStack.hpp"
#include "Module.hpp"

namespace XR
{

//==============================================================================
///@brief An application state
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

    void  SetStackSize(int size);
    void  Push(State* pState, int tFade = 0); // no ownership transfer
    void  Pop(int tFade = 0); // no ownership transfer
    void  Change(State* pState, int tFade = 0); // no ownership transfer

    void  Update(int32 tDelta);
    void  Render();
    
  private:
    // types
    typedef std::vector<State*> StateVector;
    
    // internal
    void  _ExpireState();

    // data
    StateVector m_stack;
    int32       m_iState;
    
    State*      m_pRequested; // no ownership, will be made current on Update()
    State*      m_pPrevious;  // no ownership, fading state
    bool        m_shutdownPrevious;

    int32       m_tFade;
  };

  // structors
  State();
  virtual ~State();
  
  // general
  void  Enter(int32 tFade); // when requested state becomes active
  void  Exit(int32 tFade);  // when requested state becomes inactive; must reset changes made since Enter()
  void  Update(int32 tDelta);

  // friends
  friend class  Manager;

protected:
  // data
  int32 m_tFade;

  // virtual
  virtual void  Init() =0;  // when state gets pushed on stack
  virtual void  Shutdown() =0;  // when state gets popped from stack; must reset changes since Init()

  virtual void  _Enter();
  virtual void  _Exit();
  virtual void  _Update(int32 tDelta) =0;
};

//==============================================================================
/*
  implementation
*/
//==============================================================================
inline
int32 State::Manager::GetFadeTimer() const
{
  return m_tFade;
}

} // XR

#endif // XR_STATE_HPP
