#ifndef XR_STATE_HPP
#define XR_STATE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================

#include <list>
#include <cstdint>

namespace xr
{

//==============================================================================
///@brief An application state. Life cycle:
/// Init() when pushed onto the stack,
///   Enter() when becoming the current state,
///     Update(), Render() while active
///   Exit() when a new state was requested,
/// Shutdown() when removed from the stack.
class State
{
public:
  // types
  //==============================================================================
  ///@brief Maintains a stack of states and manages their life cycles.
  class  Manager
  {
  public:
    // structors
    Manager();
    ~Manager();

    // general
    void  Push(State& s);  // no ownership transfer
    void  Change(State& s);  // no ownership transfer
    void  Pop();

    void  Update(double ms);
    void  Render();

    void  Clear();

  protected:
    // types
    typedef std::list<State*>  Stack;

    // data
    Stack  m_states;

    // internal
    void  PushInternal(State& s);
    void  PopInternal();
  };

  //==============================================================================
  ///@brief Signifies the result of a State Update() encapsulating State::Manager
  /// operation and a next State to go to.
  struct  Result
  {
    // types
    enum  Operation
    {
      NONE,
      PUSH,
      CHANGE,
      POP
    };

    // structors
    Result()
    : operation(NONE),
      state(nullptr)
    {}

    Result(Operation op, State* p = nullptr)
    : operation(op),
      state(p)
    {}

    // data
    Operation  operation;
    State*    state;
  };

  // structors
  State();
  virtual ~State();

  // general
  virtual void  Init()  {}
  virtual void  Enter() =0;
  virtual void  Update(double ms, Result& result) =0;
  virtual void  Render() =0;
  virtual void  Exit() =0;
  virtual void  Shutdown() {}
};

} // xr

#endif // XR_STATE_HPP
