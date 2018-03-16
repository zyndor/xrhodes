//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "InputImpl.hpp"
#include "XR/Device.hpp"
#include "XR/FileWriter.hpp"
#include "XR/JsonReader.hpp"
#include "XR/JsonWriter.hpp"
#include "XR/jsonutils.hpp"
#include "XR/utils.hpp"
#include "XR/stringutils.hpp"
#include "SDL_events.h"
#include "SDL.h"
#include <cstdlib>

namespace XR
{

namespace
{
//==============================================================================
static const char* const  kConfigName = "xr.json";

//==============================================================================
static void ScreenChangeEventHandler(CallbackObject::List& cbos, void* systemData)
{
  const SDL_Event*  pEvent(static_cast<SDL_Event*>(systemData));
  Device::ScreenChangeEvent e =
  {
    false,
    uint32_t(pEvent->window.data1),
    uint32_t(pEvent->window.data2)
  };

  CallbackObject::CallList(cbos, &e);
}

//==============================================================================
static struct
{
  // data
  CallbackObject::List  callbacks[static_cast<int>(Device::Event::kCount)];
  CallbackObject::List  postponedAdd[static_cast<int>(Device::Event::kCount)];
  CallbackObject::List  postponedRemove[static_cast<int>(Device::Event::kCount)];
  bool                  isQuitRequested;
  bool                  isPauseRequested;
  bool                  isYielding;
  JSON::Entity*         config;
  SDL_Window*           mainWindow;
  uint32_t              windowWidth;
  uint32_t              windowHeight;

  inline
  CallbackObject::List& GetCallbacks(Device::Event e)
  {
    return callbacks[static_cast<int>(e)];
  }
} s_deviceImpl;

}

//==============================================================================
static int  FilterEvents(void* /*userData*/, SDL_Event* pEvent)
{
  int handled = 0;
  switch (pEvent->type)
  {
  case  SDL_APP_WILLENTERBACKGROUND:
    s_deviceImpl.isPauseRequested = true;
    CallbackObject::CallList(s_deviceImpl.GetCallbacks(Device::Event::Pause), nullptr);
    break;

  case  SDL_APP_DIDENTERFOREGROUND:
    s_deviceImpl.isPauseRequested = false;
    CallbackObject::CallList(s_deviceImpl.GetCallbacks(Device::Event::Resume), nullptr);
    break;

  case  SDL_APP_TERMINATING:
    XR_ASSERT(FilterEvents, s_deviceImpl.isQuitRequested != true);
    s_deviceImpl.isQuitRequested = true;
    CallbackObject::CallList(s_deviceImpl.GetCallbacks(Device::Event::Quit), nullptr);
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
    JSON::Writer  writer;
    writer.SetLinebreaks(true);
    writer.SetIndents(true);
    writer.SetSpaceAfterColon(true);
    writer.SetAutoEscape(true);

    writer.Start(JSON::Writer::OBJECT);

    writer.WriteObject("Device").
      WriteValue("logging", true).
      CloseScope();

    writer.WriteObject("Display").
      WriteValue("width", 800).
      WriteValue("height", 600).
      WriteValue("windowed", true).
      WriteValue("vsync", true).
      CloseScope();

    writer.WriteObject("GFX").
      WriteValue("framePoolSize", 256000).
      CloseScope();

    writer.WriteObject("Input").
      WriteValue("ignoreControllers", false).
      CloseScope();

    std::string json(writer.Finish(true));

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

  uint32_t flags(SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  if (Device::GetConfigInt("Display", "windowed", false) == 0)
  {
    flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
  }

  s_deviceImpl.mainWindow = SDL_CreateWindow(title,
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    s_deviceImpl.windowWidth, s_deviceImpl.windowHeight, flags);

  // start listening to events
  SDL_AddEventWatch(FilterEvents, nullptr);
}

//==============================================================================
void * Device::GetMainWindow()
{
  return s_deviceImpl.mainWindow;
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
    if (groupName != 0)
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
      JSON::Entity* groupEntity(s_deviceImpl.config->GetChild(varName, JSON::VALUE));
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
bool Device::RegisterCallback( Event ev, Callback callback, void* userData )
{
  XR_ASSERT(Device, ev != Event::kCount);
  XR_ASSERT(Device, callback != nullptr);
  // check if already added
  const int iEv = static_cast<int>(ev);
  for (CallbackObject::List::iterator i0(s_deviceImpl.callbacks[iEv].begin()),
    i1(s_deviceImpl.callbacks[iEv].end()); i0 != i1; ++i0)
  {
    if (i0->callback == callback)
    {
      return false;
    }
  }

  if(s_deviceImpl.isYielding)
  {
    // check for a postponed add as well
    for(CallbackObject::List::iterator i0(s_deviceImpl.postponedAdd[iEv].begin()),
      i1(s_deviceImpl.postponedAdd[iEv].end()); i0 != i1; ++i0)
    {
      if(i0->callback == callback)
      {
        return false;
      }
    }

    s_deviceImpl.postponedAdd[iEv].push_back(CallbackObject(callback, userData));
  }
  else
  {
    s_deviceImpl.callbacks[iEv].push_back(CallbackObject(callback, userData));
  }
  return true;
}

//==============================================================================
bool Device::UnregisterCallback( Event ev, Callback callback )
{
  XR_ASSERT(Device, ev != Event::kCount);
  XR_ASSERT(Device, callback != nullptr);
  const int iEv = static_cast<int>(ev);
  if(s_deviceImpl.isYielding)
  {
    // if got it, add to remove list
    for(CallbackObject::List::iterator i0(s_deviceImpl.callbacks[iEv].begin()),
      i1(s_deviceImpl.callbacks[iEv].end()); i0 != i1; ++i0)
    {
      if(i0->callback == callback)
      {
        s_deviceImpl.postponedRemove[iEv].push_back(*i0);
        return true;
      }
    }

    // if on postponed add list, remove it
    for(CallbackObject::List::iterator i0(s_deviceImpl.postponedAdd[iEv].begin()),
      i1(s_deviceImpl.postponedAdd[iEv].end()); i0 != i1; ++i0)
    {
      if(i0->callback == callback)
      {
        i0 = s_deviceImpl.postponedAdd[iEv].erase(i0);
        return true;
      }
    }
  }
  else
  {
    // if got it, remove
    for(CallbackObject::List::iterator i0(s_deviceImpl.callbacks[iEv].begin()),
      i1(s_deviceImpl.callbacks[iEv].end()); i0 != i1; ++i0)
    {
      if(i0->callback == callback)
      {
        s_deviceImpl.callbacks[iEv].erase(i0);
        return true;
      }
    }
  }
  return false;
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
        CallbackObject::CallList(s_deviceImpl.GetCallbacks(Device::Event::Quit), nullptr);
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
        ScreenChangeEventHandler(s_deviceImpl.GetCallbacks(Event::ScreenChange), &e);
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

        Input::TouchActionEvent  eTouch =
        {
          // NOTE: keep an eye on these guys; the exact range of their possible values isn't clear.
          static_cast<uint32_t>(e.tfinger.touchId),
          static_cast<uint32_t>(e.tfinger.fingerId),
          x,
          y,
          e.tfinger.type == SDL_FINGERDOWN
        };
        CallbackObject::CallList(InputImpl::s_instance->GetCallbacks(Input::Event::TouchAction), &eTouch);
      }
      break;
    }

    case  SDL_FINGERMOTION:
      if (InputImpl::s_instance)
      {
        int16_t x = int16_t(std::round(e.tfinger.x * s_deviceImpl.windowWidth));
        int16_t y = int16_t(std::round(e.tfinger.y * s_deviceImpl.windowHeight));

        Input::TouchMotionEvent eTouch =
        {
          // NOTE: keep an eye on these guys; the exact range of their possible values isn't clear.
          static_cast<uint32_t>(e.tfinger.touchId),
          static_cast<uint32_t>(e.tfinger.fingerId),
          x,
          y
        };
        CallbackObject::CallList(InputImpl::s_instance->GetCallbacks(Input::Event::TouchMotion), &eTouch);
      }
      break;

    case  SDL_CONTROLLERDEVICEADDED:
    case  SDL_CONTROLLERDEVICEREMOVED:
    case  SDL_CONTROLLERDEVICEREMAPPED:
      // Input implementation
      break;
    }
  }
  s_deviceImpl.isYielding = false;

  for(int i = 0; i < static_cast<int>(Event::kCount); ++i)
  {
    Event e(static_cast<Event>(i));
    CallbackObject::List& lRemove(s_deviceImpl.postponedRemove[i]);
    while(!lRemove.empty())
    {
      XR_DEBUG_ONLY(bool result =) UnregisterCallback(e, lRemove.front().callback);
      XR_ASSERT(Device, result);
      lRemove.pop_front();
    }

    CallbackObject::List& lAdd(s_deviceImpl.postponedAdd[i]);
    while(!lAdd.empty())
    {
      CallbackObject& cbo(lAdd.front());
      XR_DEBUG_ONLY(bool result =) RegisterCallback(e, cbo.callback, cbo.userData);
      XR_ASSERT(Device, result);
      lAdd.pop_front();
    }
  }
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

  SDL_DestroyWindow(s_deviceImpl.mainWindow);
  s_deviceImpl.mainWindow = nullptr;

  SDL_Quit();

  delete s_deviceImpl.config;
  s_deviceImpl.config = nullptr;
}

} // XR
