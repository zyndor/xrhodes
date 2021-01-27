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
#include "xr/Timer.hpp"
#include <cmath>
#include <inttypes.h>

namespace xr
{
namespace
{

const double kMsInSecond = 1000.;

}

//==============================================================================
void GameLoop::Run(double frameDelayMs, double frameCapMs,
  OnUpdate const& onUpdate, OnRender const& onRender, OnSecond const* onSecond)
{
  uint32_t numUpdates = 0;
  uint32_t numRenders = 0;
  double tLast = Timer::GetUST();
  double tNextSecond = tLast + kMsInSecond;
  double tAccumulator = 0;

  while (true)
  {
    // render a frame.
    onRender.Call();
    ++numRenders;

    // update time.
    double now = Timer::GetUST();

    // process seconds - but only bother if a callback was provided.
    if (onSecond && now >= tNextSecond)
    {
      double extra = now - tNextSecond;
      const float seconds = 1000.f / float(kMsInSecond + extra);
      const float fNumUpdates = numUpdates * seconds;
      const float fNumRenders = numRenders * seconds;
      onSecond->Call(Second{ fNumUpdates, fNumRenders });
      numUpdates = 0;
      numRenders = 0;
      tNextSecond = now + kMsInSecond - std::fmod(extra, kMsInSecond);
    }

    // apply frame capping
    XR_ASSERT(GameLoop, now >= tLast);
    double delta = now - tLast;
    if (delta > frameCapMs)
    {
      XR_TRACE(GameLoop, ("Frame capped at %lf ms.", delta));
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
        tAccumulator = 0.;
        break;

      case Action::Exit:
        return;
      }
    }

    tLast = now;
  }
}

void GameLoop::Run(uint64_t frameDelayMs, uint64_t frameCapMs, OnUpdate const& onUpdate,
  OnRender const& onRender, OnSecond const* onSecond)
{
  Run(double(frameDelayMs), double(frameCapMs), onUpdate, onRender, onSecond);
}

}
