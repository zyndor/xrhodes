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
static void ScreenChangeEventHandler(CallbackObject::List& cbos, void* pSystem)
{
  const SDL_Event*  pEvent(static_cast<SDL_Event*>(pSystem));
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
  CallbackObject::List  arCallback[Device::kMaxEvents];
  CallbackObject::List  arPostponedAdd[Device::kMaxEvents];
  CallbackObject::List  arPostponedRemove[Device::kMaxEvents];
  bool                  isQuitRequested;
  bool                  isPauseRequested;
  bool                  isYielding;
  JSON::Entity*         pConfig;
  SDL_Window*           mainWindow;
  uint32_t              windowWidth;
  uint32_t              windowHeight;
} s_deviceImpl;

}

//==============================================================================
static int  FilterEvents(void* pUser, SDL_Event* pEvent)
{
  switch (pEvent->type)
  {
  case  SDL_APP_WILLENTERBACKGROUND:
    s_deviceImpl.isPauseRequested = true;
    CallbackObject::CallList(s_deviceImpl.arCallback[Device::EV_PAUSE], 0);
    return 0;
    break;

  case  SDL_APP_DIDENTERFOREGROUND:
    s_deviceImpl.isPauseRequested = false;
    CallbackObject::CallList(s_deviceImpl.arCallback[Device::EV_RESUME], 0);
    return 0;
    break;

  case  SDL_APP_TERMINATING:
    XR_ASSERT(FilterEvents, s_deviceImpl.isQuitRequested != true);
    s_deviceImpl.isQuitRequested = true;
    CallbackObject::CallList(s_deviceImpl.arCallback[Device::EV_QUIT], 0);
    return 0;
    break;
  }
  return 1;
}

//==============================================================================
void Device::Init(char const* title)
{
  bool  result(SDL_Init(SDL_INIT_EVERYTHING & ~SDL_INIT_TIMER) == 0);
  if (!result)
  {
    XR_TRACE(Device, ("Failed to initialise SDL."));
    //exit(1);
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

    writer.WriteObject("Audio").
      WriteValue("sampleRate", 22050).
      WriteValue("bufferSize", 4096).
      WriteValue("mixingChannels", 16).
      WriteValue("forceMono", false).
      CloseScope();

    writer.WriteObject("Input").
      WriteValue("ignoreControllers", false).
      CloseScope();

    std::string json(writer.Finish(true));

    FileWriter file;
    result = file.Open(kConfigName, FileWriter::Mode::Truncate, false) &&
      file.Write(json.c_str(), json.size(), 1) == 1;
  }

  s_deviceImpl.pConfig = LoadJSON(kConfigName, 64, false);

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
  SDL_AddEventWatch(FilterEvents, 0);

  Log::Init();
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
void Device::Exit()
{
  XR_ASSERT(Device, !s_deviceImpl.isYielding);

  SDL_DestroyWindow(s_deviceImpl.mainWindow);
  s_deviceImpl.mainWindow = nullptr;

  SDL_VideoQuit();

  SDL_Quit();

  delete s_deviceImpl.pConfig;
  s_deviceImpl.pConfig = 0;

  Log::Exit();
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
std::string Device::GetConfig(const char* pGroup, const char* pId)
{
  XR_ASSERT(Device, pId != 0);
  std::string result;
  if (s_deviceImpl.pConfig != 0)
  {
    if (pGroup != 0)
    {
      JSON::Entity* pEntity(s_deviceImpl.pConfig->GetChild(pGroup,
        JSON::OBJECT));
      XR_ASSERTMSG(Device, pEntity != 0, ("'%s' is not a group in root.", pGroup));
      if (pEntity != 0)
      {
        JSON::Object* pGroupData(pEntity->ToObject());
        pEntity = pGroupData->GetChild(pId, JSON::VALUE);
        XR_ASSERTMSG(Device, pEntity != 0, ("'%s' is not a value in '%s'.", pId, pGroup));
        if (pEntity != 0)
        {
          result = pEntity->ToValue()->GetValue();
        }
      }
    }
    else
    {
      JSON::Entity* pEntity(s_deviceImpl.pConfig->GetChild(pId,
        JSON::VALUE));
      XR_ASSERTMSG(Device, pEntity != 0, ("'%s' is not a value in root.", pId));
      if (pEntity != 0)
      {
        result = GetStringSafe(pEntity->GetValue());
      }
    }
  }

  return result;
}

//==============================================================================
int Device::GetConfigInt(const char* pGroup, const char* pId, int defaultValue)
{
  std::string  value(GetConfig(pGroup, pId).c_str());
  return value.empty() ? defaultValue : atoi(value.c_str());
}

//==============================================================================
bool Device::RegisterCallback( Event ev, Callback pCallback, void* pUserData )
{
  XR_ASSERT(Device, ev < kMaxEvents);
  XR_ASSERT(Device, pCallback != 0);
  // check if already added
  for (CallbackObject::List::iterator i0(s_deviceImpl.arCallback[ev].begin()),
    i1(s_deviceImpl.arCallback[ev].end()); i0 != i1; ++i0)
  {
    if (i0->pCallback == pCallback)
    {
      return false;
    }
  }

  if(s_deviceImpl.isYielding)
  {
    // check for a postponed add as well
    for(CallbackObject::List::iterator i0(s_deviceImpl.arPostponedAdd[ev].begin()),
      i1(s_deviceImpl.arPostponedAdd[ev].end()); i0 != i1; ++i0)
    {
      if(i0->pCallback == pCallback)
      {
        return false;
      }
    }

    s_deviceImpl.arPostponedAdd[ev].push_back(CallbackObject(pCallback, pUserData));
  }
  else
  {
    s_deviceImpl.arCallback[ev].push_back(CallbackObject(pCallback, pUserData));
  }
  return true;
}

//==============================================================================
bool Device::UnregisterCallback( Event ev, Callback pCallback )
{
  XR_ASSERT(Device, ev < kMaxEvents);
  XR_ASSERT(Device, pCallback != 0);
  if(s_deviceImpl.isYielding)
  {
    // if got it, add to remove list
    for(CallbackObject::List::iterator i0(s_deviceImpl.arCallback[ev].begin()),
      i1(s_deviceImpl.arCallback[ev].end()); i0 != i1; ++i0)
    {
      if(i0->pCallback == pCallback)
      {
        s_deviceImpl.arPostponedRemove[ev].push_back(*i0);
        return true;
      }
    }

    // if on postponed add list, remove it
    for(CallbackObject::List::iterator i0(s_deviceImpl.arPostponedAdd[ev].begin()),
      i1(s_deviceImpl.arPostponedAdd[ev].end()); i0 != i1; ++i0)
    {
      if(i0->pCallback == pCallback)
      {
        i0 = s_deviceImpl.arPostponedAdd[ev].erase(i0);
        return true;
      }
    }
  }
  else
  {
    // if got it, remove
    for(CallbackObject::List::iterator i0(s_deviceImpl.arCallback[ev].begin()),
      i1(s_deviceImpl.arCallback[ev].end()); i0 != i1; ++i0)
    {
      if(i0->pCallback == pCallback)
      {
        s_deviceImpl.arCallback[ev].erase(i0);
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
        CallbackObject::CallList(s_deviceImpl.arCallback[Device::EV_QUIT], 0);
      }
      break;

    case  SDL_WINDOWEVENT:
      // translate event
      switch (e.window.event)
      {
      //case  SDL_WINDOWEVENT_HIDDEN:
      case  SDL_WINDOWEVENT_LEAVE:
        e.type = SDL_APP_WILLENTERBACKGROUND;
        SDL_PushEvent(&e);
        break;

      //case  SDL_WINDOWEVENT_SHOWN:
      case  SDL_WINDOWEVENT_ENTER:
        e.type = SDL_APP_DIDENTERFOREGROUND;
        SDL_PushEvent(&e);
        break;

      case  SDL_WINDOWEVENT_CLOSE:
        e.type = SDL_APP_TERMINATING;
        SDL_PushEvent(&e);
        break;

      case  SDL_WINDOWEVENT_RESIZED:
        ScreenChangeEventHandler(s_deviceImpl.arCallback[EV_SCREEN_CHANGE], &e);
        break;
      }
      break;

    case  SDL_KEYDOWN:
    case  SDL_KEYUP:
    {
      if (InputImpl::s_pInstance)
      {
        Input::KeyEvent eKey =
        {
          TranslateKeyCodeNative(e.key.keysym.scancode),
          e.key.state == SDL_PRESSED
        };
        CallbackObject::CallList(InputImpl::s_pInstance->arCallback[Input::EV_KEY], &eKey);
      }
      break;
    }

    case  SDL_MOUSEMOTION:
    {
      if (InputImpl::s_pInstance)
      {
        Input::MouseMotionEvent  eMouse =
        {
          e.motion.which,
          static_cast<int16_t>(e.motion.x),
          static_cast<int16_t>(e.motion.y)
        };
        CallbackObject::CallList(InputImpl::s_pInstance->arCallback[Input::EV_MOUSE_MOTION], &eMouse);
      }
      break;
    }

    case  SDL_MOUSEBUTTONDOWN:
    case  SDL_MOUSEBUTTONUP:
    case  SDL_MOUSEWHEEL:
    {
      if (InputImpl::s_pInstance)
      {
        Input::MouseActionEvent  eMouse =
        {
          e.button.which,
          TranslateMouseButtonNative(e.button.button),
          static_cast<int16_t>(e.button.x),
          static_cast<int16_t>(e.button.y),
          e.button.state == SDL_PRESSED
        };
        CallbackObject::CallList(InputImpl::s_pInstance->arCallback[Input::EV_MOUSE_ACTION], &eMouse);
      }
      break;
    }

    case  SDL_FINGERUP:
    case  SDL_FINGERDOWN:
    {
      if (InputImpl::s_pInstance)
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
        CallbackObject::CallList(InputImpl::s_pInstance->arCallback[Input::EV_TOUCH_ACTION], &eTouch);
      }
      break;
    }

    case  SDL_FINGERMOTION:
    {
      if (InputImpl::s_pInstance)
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
        CallbackObject::CallList(InputImpl::s_pInstance->arCallback[Input::EV_TOUCH_MOTION], &eTouch);
        break;
      }
    }

    case  SDL_CONTROLLERDEVICEADDED:
    case  SDL_CONTROLLERDEVICEREMOVED:
    case  SDL_CONTROLLERDEVICEREMAPPED:
      // Input implementation
      break;
    }
  }
  s_deviceImpl.isYielding = false;

  for(int i = 0; i < kMaxEvents; ++i)
  {
    Event e(static_cast<Event>(i));
    CallbackObject::List& lRemove(s_deviceImpl.arPostponedRemove[i]);
    while(!lRemove.empty())
    {
      XR_DEBUG_ONLY(bool result =) UnregisterCallback(e, lRemove.front().pCallback);
      XR_ASSERT(Device, result);
      lRemove.pop_front();
    }

    CallbackObject::List& lAdd(s_deviceImpl.arPostponedAdd[i]);
    while(!lAdd.empty())
    {
      CallbackObject& cbo(lAdd.front());
      XR_DEBUG_ONLY(bool result =) RegisterCallback(e, cbo.pCallback, cbo.pUserData);
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

} // XR
