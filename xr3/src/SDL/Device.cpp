//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "InputImpl.hpp"
#include "GfxContext.hpp"
#include "xr/Config.hpp"
#include "xr/Device.hpp"
#include "xr/events/SignalBroadcaster.hpp"
#include "xr/strings/stringutils.hpp"
#include "SDL_events.h"
#include "SDL.h"

namespace xr
{

namespace
{
//==============================================================================
static struct
{
  // data
  SignalBroadcaster<> onPause;
  SignalBroadcaster<> onResume;
  SignalBroadcaster<> onQuit;
  SignalBroadcaster<Device::ScreenChangeData const&> onScreenChange;

  bool  isQuitRequested;
  bool  isPauseRequested;
  bool  isYielding;

  SDL_Window* mainWindow;
  uint32_t  windowWidth;
  uint32_t  windowHeight;
  Gfx::Context* gfxContext;
} s_deviceImpl;

// TODO: deprecate with GetConfig*().
std::string ConcatGroupVarNames(char const* groupName, char const* varName)
{
  std::string result(GetStringSafe(groupName));
  if (!result.empty())
  {
    result += "_";
  }
  return result + varName;
}

}

//==============================================================================
static int  FilterEvents(void* /*userData*/, SDL_Event* event)
{
  int handled = 0;
  switch (event->type)
  {
  case  SDL_APP_WILLENTERBACKGROUND:
    s_deviceImpl.isPauseRequested = true;
    s_deviceImpl.onPause.Broadcast();
    break;

  case  SDL_APP_DIDENTERFOREGROUND:
    s_deviceImpl.isPauseRequested = false;
    s_deviceImpl.onResume.Broadcast();
    break;

  case  SDL_APP_TERMINATING:
    XR_ASSERT(FilterEvents, s_deviceImpl.isQuitRequested != true);
    s_deviceImpl.isQuitRequested = true;
    s_deviceImpl.onQuit.Broadcast();
    break;

  default:
    handled = 1;
    break;
  }
  return handled;
}

//==============================================================================
void Device::Init(char const* title)
{
  bool  result(SDL_Init(SDL_INIT_EVERYTHING & ~SDL_INIT_TIMER) == 0);
  if (!result)
  {
    XR_TRACE(Device, ("Failed to initialise SDL."));
    exit(1);
  }

  s_deviceImpl.isPauseRequested = false;
  s_deviceImpl.isQuitRequested = false;
  s_deviceImpl.isYielding = false;

  // create window
  if (!title)
  {
    title = "XRhodes Application";
  }

  int windowWidth = Config::GetInt("XR_DISPLAY_WIDTH", 800);
  int windowHeight = Config::GetInt("XR_DISPLAY_HEIGHT", 600);

  uint32_t flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
  if (Config::GetInt("XR_DISPLAY_FULLSCREEN", false) == 1)
  {
    flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
  }

  s_deviceImpl.mainWindow = SDL_CreateWindow(title,
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    windowWidth, windowHeight, flags);

  SDL_GetWindowSize(s_deviceImpl.mainWindow, &windowWidth, &windowHeight);
  s_deviceImpl.windowWidth = windowWidth;
  s_deviceImpl.windowHeight = windowHeight;

  s_deviceImpl.gfxContext = new Gfx::Context(*s_deviceImpl.mainWindow);

  // start listening to events
  SDL_AddEventWatch(FilterEvents, nullptr);
}

//==============================================================================
Gfx::Context* Device::GetGfxContext()
{
  return s_deviceImpl.gfxContext;
}

//==============================================================================
void Device::SetMainWindowTitle(char const* title)
{
  XR_ASSERT(Device, title != nullptr);
  XR_ASSERT(Device, s_deviceImpl.mainWindow != nullptr);
  SDL_SetWindowTitle(s_deviceImpl.mainWindow, title);
}

//==============================================================================
bool Device::IsQuitting()
{
  return s_deviceImpl.isQuitRequested;
}

//==============================================================================
bool Device::IsPaused()
{
  return s_deviceImpl.isPauseRequested;
}

//==============================================================================
std::string Device::GetConfig(char const* groupName, char const* varName)
{
  std::string groupVarName = ConcatGroupVarNames(groupName, varName);
  return Config::Get(groupVarName.c_str());
}

//==============================================================================
int Device::GetConfigInt(char const* groupName, char const* varName, int defaultValue)
{
  std::string groupVarName = ConcatGroupVarNames(groupName, varName);
  return Config::GetInt(groupVarName.c_str(), defaultValue);
}

//==============================================================================
Signal<void>& Device::SuspendSignal()
{
  return s_deviceImpl.onPause;
}

//==============================================================================
Signal<void>& Device::ResumeSignal()
{
  return s_deviceImpl.onResume;
}

//==============================================================================
Signal<void>& Device::QuitSignal()
{
  return s_deviceImpl.onQuit;
}

//==============================================================================
Signal<void, Device::ScreenChangeData const&>& Device::ScreenChangeSignal()
{
  return s_deviceImpl.onScreenChange;
}

//==============================================================================
void  Device::YieldOS(int32_t ms)
{
  XR_ASSERT(Device, !s_deviceImpl.isYielding);
  s_deviceImpl.isYielding = true;
  SDL_Event e;
  while (SDL_PollEvent(&e) != 0)
  {
    switch (e.type)
    {
    case  SDL_QUIT:
      if (!s_deviceImpl.isQuitRequested)
      {
        s_deviceImpl.isQuitRequested = true;
        s_deviceImpl.onQuit.Broadcast();
      }
      break;

    case  SDL_WINDOWEVENT:
      // translate event
      switch (e.window.event)
      {
      case  SDL_WINDOWEVENT_FOCUS_LOST:
        e.type = SDL_APP_WILLENTERBACKGROUND;
        SDL_PushEvent(&e);
        break;

      case  SDL_WINDOWEVENT_FOCUS_GAINED:
        e.type = SDL_APP_DIDENTERFOREGROUND;
        SDL_PushEvent(&e);
        break;

      case  SDL_WINDOWEVENT_CLOSE:
        e.type = SDL_APP_TERMINATING;
        SDL_PushEvent(&e);
        break;

      case  SDL_WINDOWEVENT_RESIZED:
        s_deviceImpl.onScreenChange.Broadcast(Device::ScreenChangeData{
          false,
          uint32_t(e.window.data1),
          uint32_t(e.window.data2)
        });
        break;
      }
      break;

    case  SDL_KEYDOWN:
    case  SDL_KEYUP:
      if (auto input = InputImpl::Instance())
      {
        input->OnKeyEvent(e.key);
      }
      break;

    case  SDL_MOUSEMOTION:
      if (auto input = InputImpl::Instance())
      {
        input->OnMouseMotion(e.motion);
      }
      break;

    case  SDL_MOUSEBUTTONDOWN:
    case  SDL_MOUSEBUTTONUP:
      if (auto input = InputImpl::Instance())
      {
        input->OnMouseAction(e.button);
      }
      break;

    case  SDL_MOUSEWHEEL:
      if (auto input = InputImpl::Instance())
      {
        input->OnMouseWheel(e.wheel);
      }
      break;

    case  SDL_FINGERUP:
    case  SDL_FINGERDOWN:
    {
      if (auto input = InputImpl::Instance())
      {
        int16_t x = int16_t(std::round(e.tfinger.x * s_deviceImpl.windowWidth));
        int16_t y = int16_t(std::round(e.tfinger.y * s_deviceImpl.windowHeight));

        Input::TouchActionData  eTouch =
        {
          // NOTE: keep an eye on these guys; the exact range of their possible values isn't clear.
          static_cast<uint32_t>(e.tfinger.touchId),
          static_cast<uint32_t>(e.tfinger.fingerId),
          x,
          y,
          e.tfinger.type == SDL_FINGERDOWN
        };
        input->onTouchAction.Broadcast(eTouch);
      }
      break;
    }

    case  SDL_FINGERMOTION:
      if (auto input = InputImpl::Instance())
      {
        int16_t x = int16_t(std::round(e.tfinger.x * s_deviceImpl.windowWidth));
        int16_t y = int16_t(std::round(e.tfinger.y * s_deviceImpl.windowHeight));

        Input::TouchMotionData eTouch =
        {
          // NOTE: keep an eye on these guys; the exact range of their possible values isn't clear.
          static_cast<uint32_t>(e.tfinger.touchId),
          static_cast<uint32_t>(e.tfinger.fingerId),
          x,
          y
        };

        input->onTouchMotion.Broadcast(eTouch);
      }
      break;

    case  SDL_JOYDEVICEADDED:
      if (auto input = InputImpl::Instance())
      {
        input->OpenController(e.jdevice.which);
      }
      break;

    case  SDL_JOYDEVICEREMOVED:
      if (auto input = InputImpl::Instance())
      {
        input->CloseController(e.jdevice.which);
      }
      break;

    case  SDL_JOYAXISMOTION:
      if (auto input = InputImpl::Instance())
      {
        input->OnJoyAxis(e.jaxis);
      }
      break;

    case  SDL_JOYBUTTONDOWN:
    case  SDL_JOYBUTTONUP:
      if (auto input = InputImpl::Instance())
      {
        input->OnJoyButton(e.jbutton);
      }
      break;

    case  SDL_JOYHATMOTION:
      if (auto input = InputImpl::Instance())
      {
        input->OnJoyHat(e.jhat);
      }
      break;

    case  SDL_JOYBALLMOTION:
      if (auto input = InputImpl::Instance())
      {
        input->OnJoyBall(e.jball);
      }
      break;

    case  SDL_CONTROLLERDEVICEADDED:
    case  SDL_CONTROLLERDEVICEREMOVED:
    case  SDL_CONTROLLERDEVICEREMAPPED:
      // TODO?
      break;
    }
  }
  s_deviceImpl.isYielding = false;
}

//==============================================================================
bool Device::IsYielding()
{
  return s_deviceImpl.isYielding;
}

//==============================================================================
void Device::Shutdown()
{
  XR_ASSERT(Device, !s_deviceImpl.isYielding);

  delete s_deviceImpl.gfxContext;
  s_deviceImpl.gfxContext = nullptr;

  SDL_DestroyWindow(s_deviceImpl.mainWindow);
  s_deviceImpl.mainWindow = nullptr;

  SDL_Quit();
}

} // XR
