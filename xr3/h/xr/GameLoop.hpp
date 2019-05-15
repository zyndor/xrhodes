#ifndef XR_GAMELOOP_HPP
#define XR_GAMELOOP_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/events/Callback.hpp"
#include "xr/types/fundamentals.hpp"
#include <cstdint>

namespace xr
{

//==============================================================================
///@brief Implements a canonical game loop, calling out to update, render, and
/// (optionally) per second callbacks. The frequency of updates is fixed, and
/// capping is applied when a frame takes too long.
///@note Calls xr::Device::YieldOS(), so the callbacks from subsystems are
/// processed. For an event
class GameLoop
{
  XR_NONOBJECT_DECL(GameLoop)

public:
  // types
  ///@brief Update / render data relating to the last second.
  struct Second
  {
    float updateCount;
    float renderCount;
  };

  ///@brief The action that will be taken following an update which it
  /// originates from.
  enum class Action
  {
    Continue, // Carry on as you were.
    Skip,  // Skips remaining updates, proceeds to the next render (unless otherwise terminated);
    Exit,  // The game loop is terminated.
  };

  ///@brief An update function. Action& is an output parameter signifying
  /// the action that should be taken following the return from the function;
  /// its default value is Action::Continue.
  using OnUpdate = Callback<void, Action&>;

  ///@brief A render function.
  using OnRender = Callback<void>;

  ///@brief Callback to call when a second has lapsed.
  using OnSecond = Callback<void, Second>;

  // general
  ///@brief Runs the game loop synchronously. Returns when the Device is
  /// quitting, or termination was requested by the update callback.
  ///@param frameDelayMs the desired frequency of updates, in milliseconds.
  ///@param frameCapMs the maximum amount of time allowed between updates,
  /// in milliseconds.
  ///@param onUpdate Update callback.
  ///@param onSecond Render callback.
  ///@param onSecond Callback to call every second. Optional.
  ///@note onSecond is not Clone()d; it will need to stay valid until Run()
  /// returns.
  static void Run(uint64_t frameDelayMs, uint64_t frameCapMs,
    OnUpdate const& onUpdate, OnRender const& onRender,
    OnSecond const* onSecond = nullptr);
};

}

#endif //XR_GAMELOOP_HPP
