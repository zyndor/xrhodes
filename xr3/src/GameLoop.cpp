//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/GameLoop.hpp"
#include "xr/Device.hpp"
#include "xr/Input.hpp"
#include "xr/Timer.hpp"
#include <inttypes.h>

namespace xr
{
namespace
{

const uint32_t kMsInSecond = 1000;

}

//==============================================================================
void GameLoop::Run(uint64_t frameDelayMs, uint64_t frameCapMs,
  OnUpdate const& onUpdate, OnRender const& onRender, OnSecond const* onSecond)
{
  uint32_t numUpdates = 0;
  uint32_t numRenders = 0;
  uint64_t tLast = Timer::GetUST();
  uint64_t tNextSecond = tLast + kMsInSecond;
  uint64_t tAccumulator = 0;

  while (true)
  {
    // render a frame.
    onRender.Call();
    ++numRenders;

    // update time.
    uint64_t now = Timer::GetUST();

    // process seconds - but only bother if a callback was provided.
    if (onSecond && now >= tNextSecond)
    {
      uint64_t extra = now - tNextSecond;
      const float seconds = 1000.f / (kMsInSecond + extra);
      const float fNumUpdates = numUpdates * seconds;
      const float fNumRenders = numRenders * seconds;
      onSecond->Call(Second{ fNumUpdates, fNumRenders });
      numUpdates = 0;
      numRenders = 0;
      tNextSecond = now + kMsInSecond - (extra % kMsInSecond);
    }

    // apply frame capping
    XR_ASSERT(GameLoop, now >= tLast);
    uint64_t delta = now - tLast;
    if (delta > frameCapMs)
    {
      XR_TRACE(GameLoop, ("Frame capped at %" PRIu64 "ms.", delta));
      delta = frameCapMs;
    }

    // updates
    tAccumulator += delta;
    while (tAccumulator >= frameDelayMs)
    {
      // process events and update input
      Device::YieldOS(0);
      if (Device::IsQuitting())
      {
        return;
      }

      // do the update.
      Action action = Action::Continue;
      onUpdate.Call(action);
      ++numUpdates;
      switch (action)
      {
      case Action::Continue:
        tAccumulator -= frameDelayMs;
        break;

      case Action::Skip:
        tAccumulator = 0;
        break;

      case Action::Exit:
        return;
      }
    }

    tLast = now;
  }
}

}
