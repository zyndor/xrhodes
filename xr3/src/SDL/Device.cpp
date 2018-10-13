//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "InputImpl.hpp"
#include "xr/Device.hpp"
#include "xr/FileWriter.hpp"
#include "xr/JsonReader.hpp"
#include "xr/JsonWriter.hpp"
#include "xr/jsonutils.hpp"
#include "xr/SignalBroadcaster.hpp"
#include "xr/utils.hpp"
#include "xr/stringutils.hpp"
#include "GfxContext.hpp"
#include "SDL_events.h"
#include "SDL.h"
#include <cstdlib>

namespace xr
{

namespace
{
//==============================================================================
static const char* const  kConfigName = "xr.json";

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

  JSON::Entity* config;

  SDL_Window* mainWindow;
  uint32_t  windowWidth;
  uint32_t  windowHeight;
  Gfx::Context* gfxContext;
} s_deviceImpl;

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

  if (!File::CheckExists(kConfigName))
  {
    std::ostringstream cfgStream;
    JSON::Writer  writer(cfgStream);
    writer.SetLinebreaks(true);
    writer.SetIndent("  ");
    writer.SetSpaces(true);
    writer.SetAutoEscape(true);

    {
      auto root = writer.OpenObject();

      root.OpenObject("Device").
        WriteValue("logging", true);

      root.OpenObject("Display").
        WriteValue("width", 800).
        WriteValue("height", 600).
        WriteValue("windowed", true).
        WriteValue("vsync", true);

      root.OpenObject("GFX").
        WriteValue("framePoolSize", 256000);

      root.OpenObject("Input").
        WriteValue("ignoreControllers", false);
    }

    std::string json = cfgStream.str();
    FileWriter file;
    result = file.Open(kConfigName, FileWriter::Mode::Truncate, false) &&
      file.Write(json.c_str(), json.size(), 1) == 1;
    if (!result)
    {
      XR_TRACE(Device, ("Failed to write xr.json"));
      exit(1);
    }
  }

  s_deviceImpl.config = LoadJSON(kConfigName, 64, false);

  // create window
  if (!title)
  {
    title = "XRhodes Application";
  }

  s_deviceImpl.windowWidth = Device::GetConfigInt("Display", "width", 800);
  s_deviceImpl.windowHeight = Device::GetConfigInt("Display", "height", 600);

  uint32_t flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
  if (Device::GetConfigInt("Display", "windowed", false) == 0)
  {
    flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
  }

  s_deviceImpl.mainWindow = SDL_CreateWindow(title,
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    s_deviceImpl.windowWidth, s_deviceImpl.windowHeight, flags);

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
std::string Device::GetConfig(const char* groupName, const char* varName)
{
  XR_ASSERT(Device, varName != nullptr);
  std::string result;
  if (s_deviceImpl.config)
  {
    if (groupName != nullptr)
    {
      JSON::Entity* groupEntity = s_deviceImpl.config->GetChild(groupName, JSON::OBJECT);
      XR_ASSERTMSG(Device, groupEntity != nullptr, ("'%s' is not a groupName in root.",
        groupName));
      if (groupEntity != nullptr)
      {
        JSON::Object* groupObject = groupEntity->ToObject();
        groupEntity = groupObject->GetChild(varName, JSON::VALUE);
        XR_ASSERTMSG(Device, groupEntity != nullptr, ("'%s' is not a value in '%s'.",
          varName, groupName));
        if (groupEntity != nullptr)
        {
          result = groupEntity->ToValue()->GetValue();
        }
      }
    }
    else
    {
      JSON::Entity* groupEntity = s_deviceImpl.config->GetChild(varName, JSON::VALUE);
      XR_ASSERTMSG(Device, groupEntity != nullptr, ("'%s' is not a value in root.",
        varName));
      if (groupEntity != nullptr)
      {
        result = GetStringSafe(groupEntity->GetValue());
      }
    }
  }

  return result;
}

//==============================================================================
int Device::GetConfigInt(const char* groupName, const char* varName, int defaultValue)
{
  std::string  value(GetConfig(groupName, varName).c_str());
  return value.empty() ? defaultValue : atoi(value.c_str());
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
      if (InputImpl::s_instance)
      {
        InputImpl::s_instance->OnKeyEvent(e.key);
      }
      break;

    case  SDL_MOUSEMOTION:
      if (InputImpl::s_instance)
      {
        InputImpl::s_instance->OnMouseMotion(e.motion);
      }
      break;

    case  SDL_MOUSEBUTTONDOWN:
    case  SDL_MOUSEBUTTONUP:
      if (InputImpl::s_instance)
      {
        InputImpl::s_instance->OnMouseAction(e.button);
      }
      break;

    case  SDL_MOUSEWHEEL:
      if (InputImpl::s_instance)
      {
        InputImpl::s_instance->OnMouseWheel(e.wheel);
      }
      break;

    case  SDL_FINGERUP:
    case  SDL_FINGERDOWN:
    {
      if (InputImpl::s_instance)
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
        InputImpl::s_instance->onTouchAction.Broadcast(eTouch);
      }
      break;
    }

    case  SDL_FINGERMOTION:
      if (InputImpl::s_instance)
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

        InputImpl::s_instance->onTouchMotion.Broadcast(eTouch);
      }
      break;

    case  SDL_JOYDEVICEADDED:
      if (InputImpl::s_instance)
      {
        InputImpl::s_instance->OpenController(e.jdevice.which);
      }
      break;

    case  SDL_JOYDEVICEREMOVED:
      if (InputImpl::s_instance)
      {
        InputImpl::s_instance->CloseController(e.jdevice.which);
      }
      break;

    case  SDL_JOYAXISMOTION:
      if (InputImpl::s_instance)
      {
        InputImpl::s_instance->OnJoyAxis(e.jaxis);
      }
      break;

    case  SDL_JOYBUTTONDOWN:
    case  SDL_JOYBUTTONUP:
      if (InputImpl::s_instance)
      {
        InputImpl::s_instance->OnJoyButton(e.jbutton);
      }
      break;

    case  SDL_JOYHATMOTION:
      if (InputImpl::s_instance)
      {
        InputImpl::s_instance->OnJoyHat(e.jhat);
      }
      break;

    case  SDL_JOYBALLMOTION:
      if (InputImpl::s_instance)
      {
        InputImpl::s_instance->OnJoyBall(e.jball);
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

  delete s_deviceImpl.config;
  s_deviceImpl.config = nullptr;
}

} // XR
