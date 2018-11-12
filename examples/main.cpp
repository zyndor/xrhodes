//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "Example.hpp"
#include "xr/Device.hpp"
#include "xr/Input.hpp"
#include "xr/Timer.hpp"
#include "xr/Gfx.hpp"
#include <cstdlib>

using namespace xr;

const uint64_t kFrameDelayMs = 1000 / 60.f;
const uint64_t kFrameCappingMs = 100;

class Application
{
public:
  Application(int argc, char** argv)
  {
    // Initialise commonly used subsystems.
    File::Init({ ".", DATA_PATH });
    Device::Init();
    Input::Init();

    // Register event handlers for pause and resume.
    Device::SuspendSignal().Connect(MakeCallback(*this, &Application::OnPause));
    Device::ResumeSignal().Connect(MakeCallback(*this, &Application::OnResume));

    // Register event handlers for keys
    Input::KeySignal().Connect(MakeCallback(*this, &Application::OnKey));

    // If a command line argument was provided, try to find an example whose
    // name contains it.
    Example* example = Example::GetFirst();
    if (argc > 1)
    {
      while (example && !strstr(example->GetName().c_str(), argv[1]))
      {
        example = example->GetNext();
      }

      if (!example)
      {
        XR_TRACE(Application, ("Failed to find example '%s', using default.", argv[0]));
        example = Example::GetFirst();
      }
    }

    // Switch to it and run it.
    SwitchExample(example);
  }

  ~Application()
  {
    // Unregister callbacks. This is normally only necessary when switching
    // between multiple modules with their own application handling; here
    // we will be shutting down anyway.
    Input::KeySignal().Disconnect(MakeCallback(*this, &Application::OnKey));
    Device::SuspendSignal().Disconnect(MakeCallback(*this, &Application::OnPause));
    Device::ResumeSignal().Disconnect(MakeCallback(*this, &Application::OnResume));

    // Shutdown subsystems.
    Input::Shutdown();
    Device::Shutdown();
    File::Shutdown();
  }

  int Run()
  {
    uint32_t numUpdates = 0;
    uint32_t numRenders = 0;
    uint64_t tLast = Timer::GetUST();
    uint64_t tNow = tLast;
    uint64_t tSecond = tNow + 1000;
    int64_t tAccum = kFrameDelayMs;

    while (!(Device::IsQuitting() || m_quitRequested))
    {
      // Accumulate delay by rendering (only if not paused).
      if (!m_paused)
      {
        if (m_activeExample)
        {
          m_activeExample->Render();
          ++numRenders;
        }

        if (tLast >= tSecond)
        {
          HardString<32> stats;
          stats += " (";
          stats += numUpdates;
          stats += " / ";
          stats += numRenders;
          stats += ")";
          Device::SetMainWindowTitle((GetTitle() + stats.c_str()).c_str());
          numUpdates = 0;
          numRenders = 0;

          tSecond += 1000;
        }
      }

      tLast = tNow;
      tNow = Timer::GetUST();

      auto tDelta = std::min(tNow - tLast, kFrameCappingMs);
      tAccum += tDelta;

      // Spend time by updating the state.
      while(tAccum > 0)
      {
        // Yield to OS for event processing and handle quitting.
        Device::YieldOS(0);
        if (Device::IsQuitting())
        {
          break;
        }

        // Update input and (if not paused,) example.
        Input::Update();
        if (!m_paused && m_activeExample)
        {
          m_activeExample->Update();
        }

        ++numUpdates;
        tAccum -= kFrameDelayMs;
      }

      if (m_requestedExample)
      {
        SwitchExample(m_requestedExample);
        m_requestedExample = nullptr;
      }
    }

    if (m_activeExample)
    {
      m_activeExample->Shutdown();
      m_activeExample = nullptr;
    }
    return 0;
  }

private:
  bool m_quitRequested = false;
  bool m_paused = false;
  Example* m_activeExample = nullptr; // no ownership
  Example* m_requestedExample = nullptr;

  void SwitchExample(Example* e)
  {
    // Shutdown previous example, if any.
    if (m_activeExample)
    {
      m_activeExample->Shutdown();
    }

    // Start new example, if any.
    if (e)
    {
      e->Init();
      m_activeExample = e;
    }

    Device::SetMainWindowTitle(GetTitle().c_str());
  }

  std::string GetTitle() const
  {
    std::string title{ "XRhodes Examples" };
    if (m_activeExample)
    {
      title += ": " + m_activeExample->GetName();
    }

    if (m_paused)
    {
      title += " [PAUSED]";
    }
    return title;
  }

  void OnPause()
  {
    m_paused = true;
    Device::SetMainWindowTitle(GetTitle().c_str());
  }

  void OnResume()
  {
    Device::SetMainWindowTitle(GetTitle().c_str());
    m_paused = false;
  }

  void OnKey(Input::KeyData const& e)
  {
    if (!e.isPressed) // when the key is released.
    {
      switch(e.key)
      {
      case K_ESC:
        m_quitRequested = true;
        break;

      case K_LBRACKET:
        m_requestedExample = m_activeExample->GetPrevious();
        break;

      case K_RBRACKET:
        m_requestedExample = m_activeExample->GetNext();
        break;

      case K_F5:
        m_requestedExample = m_activeExample;

      default:
        break;
      }
    }
  }
};

int main(int argc, char** argv)
{
  // Set a few environment variables that XRhodes will use during its initialisation.
  putenv("XR_DISPLAY_FULLSCREEN=0");  // this is the default
  putenv("XR_DISPLAY_WIDTH=1024");
  putenv("XR_DISPLAY_VSYNC=1");

  return Application(argc, argv).Run();
}
