#include <cstdlib>
#include <SDL2/SDL_events.h>
#include "Device.hpp"
#include "JsonReader.hpp"
#include "JsonWriter.hpp"
#include "InputImpl.hpp"
#include "Renderer.hpp"

namespace XR
{

//==============================================================================
static const char*  kConfigName = "xr.json";

//==============================================================================
static void ScreenChangeEventHandler(CallbackObject::List& cbos, void* pSystem)
{
  const SDL_Event*  pEvent(static_cast<SDL_Event*>(pSystem));
  Device::ScreenChangeEvent e =
  {
    false,
    pEvent->window.data1,
    pEvent->window.data2
  };

  CallbackObject::CallList(cbos, &e);
}

//==============================================================================
static struct  
{
  // data
  CallbackObject::List  arCallback[Device::kMaxEvents];
  bool                  isQuitRequested;
  bool                  isPauseRequested;
  bool                  isYielding;
  JSON::Entity*         pConfig;
} s_deviceImpl;

//==============================================================================
static int  FilterEvents(void* pUser, SDL_Event* pEvent)
{
  switch(pEvent->type)
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
void Device::Init()
{
  File::Init();

  bool  result(SDL_Init(SDL_INIT_EVERYTHING) == 0);
  if(!result)
  {
    XR_TRACE(Device, ("Failed to initialise SDL."));
    //exit(1);
  }
  
  s_deviceImpl.isPauseRequested = false;
  s_deviceImpl.isQuitRequested = false;
  s_deviceImpl.isYielding = false;
  
  if(!File::CheckExists(kConfigName))
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
    
    writer.WriteObject("GFX").
      WriteValue("caption", "XRhodes Application").
      WriteValue("windowed", true).
      WriteValue("vsync", true).
      WriteValue("width", 800).
      WriteValue("height", 600).
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
    
    int hFile(File::Open(kConfigName, "wb"));
    result = hFile != File::INVALID_HANDLE &&
      File::Write(json.c_str(), json.size(), 1, hFile) > 0;
    
    File::Close(hFile);
  }

  s_deviceImpl.pConfig = JSON::LoadJSON(kConfigName);
  
  SDL_AddEventWatch(FilterEvents, 0);

  Log::Init();
}

//==============================================================================
void Device::Exit()
{
  SDL_Quit();
  
  delete s_deviceImpl.pConfig;
  s_deviceImpl.pConfig = 0;
  
  Log::Exit();
  File::Exit();
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
const char* Device::GetConfig(const char* pGroup, const char* pId)
{
  XR_ASSERT(Device, pId != 0);
  const char* pResult(0);
  if(s_deviceImpl.pConfig != 0)
  {
    if(pGroup != 0)
    {
      JSON::Entity* pEntity(s_deviceImpl.pConfig->GetChild(pGroup,
        JSON::Entity::OBJECT));
      XR_ASSERTMSG(Device, pEntity != 0, ("'%s' is not a group in root.", pGroup));
      if(pEntity != 0)
      {
        JSON::Object* pGroupData(pEntity->ToObject());
        pEntity = pGroupData->GetChild(pId, JSON::Entity::VALUE);
        XR_ASSERTMSG(Device, pEntity != 0, ("'%s' is not a value in '%s'.", pId, pGroup));
        if(pEntity != 0)
        {
          pResult = pEntity->ToValue()->GetValue();
        }
      }
    }
    else
    {
      JSON::Entity* pEntity(s_deviceImpl.pConfig->GetChild(pId,
        JSON::Entity::VALUE));
      XR_ASSERTMSG(Device, pEntity != 0, ("'%s' is not a value in root.", pId));
      if(pEntity != 0)
      {
        pResult = pEntity->ToValue()->GetValue();
      }
    }
  }
  return pResult;
}

//==============================================================================
int Device::GetConfigInt(const char* pGroup, const char* pId, int defaultValue)
{
  const char* pValue(GetConfig(pGroup, pId));
  return pValue != 0 ? atoi(pValue) : defaultValue;
}

//==============================================================================
bool Device::RegisterCallback( Event ev, Callback pCb, void* pCbData )
{
  XR_ASSERT(Device, ev < kMaxEvents);
  XR_ASSERT(Device, pCb != 0);
  for(CallbackObject::List::iterator i0(s_deviceImpl.arCallback[ev].begin()),
    i1(s_deviceImpl.arCallback[ev].end()); i0 != i1; ++i0)
  {
    if(i0->pCb == pCb)
    {
      return false;
    }
  }

  s_deviceImpl.arCallback[ev].push_back(CallbackObject(pCb, pCbData));
  return true;
}

//==============================================================================
bool Device::UnregisterCallback( Event ev, Callback pCb )
{
  XR_ASSERT(Device, ev < kMaxEvents);
  XR_ASSERT(Device, pCb != 0);
  for(CallbackObject::List::iterator i0(s_deviceImpl.arCallback[ev].begin()),
    i1(s_deviceImpl.arCallback[ev].end()); i0 != i1; ++i0)
  {
    if(i0->pCb == pCb)
    {
      s_deviceImpl.arCallback[ev].erase(i0);
      return true;
    }
  }
  return false;
}

//==============================================================================
void  Device::YieldOS(int32 ms)
{
  s_deviceImpl.isYielding = true;
  SDL_Event e;
  while(SDL_PollEvent(&e) != 0)
  {
    switch(e.type)
    {
    case  SDL_QUIT:
      if(!s_deviceImpl.isQuitRequested)
      {
        s_deviceImpl.isQuitRequested = true;
        CallbackObject::CallList(s_deviceImpl.arCallback[Device::EV_QUIT], 0);
      }
      break;

    case  SDL_WINDOWEVENT:
      // translate event
      switch(e.window.event)
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
      Input::KeyEvent eKey =
      {
        TranslateKeyCodeNative(e.key.keysym.scancode),
        e.key.state == SDL_PRESSED
      };
      CallbackObject::CallList(InputImpl::s_pInstance->arCallback[Input::EV_KEY], &eKey);
      break;
    }
      
    case  SDL_MOUSEMOTION:
    {
      Input::MouseMotionEvent  eMouse =
      {
        e.motion.which,
        e.motion.x,
        e.motion.y
      };
      CallbackObject::CallList(InputImpl::s_pInstance->arCallback[Input::EV_MOUSE_MOTION], &eMouse);
      break;
    }
      
    case  SDL_MOUSEBUTTONDOWN:
    case  SDL_MOUSEBUTTONUP:
    case  SDL_MOUSEWHEEL:
    {
      Input::MouseActionEvent  eMouse =
      {
        e.button.which,
        TranslateMouseButtonNative(e.button.button),
        e.button.x,
        e.button.y,
        e.button.state == SDL_PRESSED
      };
      CallbackObject::CallList(InputImpl::s_pInstance->arCallback[Input::EV_MOUSE_ACTION], &eMouse);
      break;
    }
      
    case  SDL_FINGERUP:
    case  SDL_FINGERDOWN:
    {
      int16 x((int16)Round(e.tfinger.x * Renderer::GetScreenWidth()));
      int16 y((int16)Round(e.tfinger.y * Renderer::GetScreenHeight()));

      Input::TouchActionEvent  eTouch =
      {
        e.tfinger.touchId,
        e.tfinger.fingerId,
        x,
        y,
        e.tfinger.type == SDL_FINGERDOWN
      };
      CallbackObject::CallList(InputImpl::s_pInstance->arCallback[Input::EV_TOUCH_ACTION], &eTouch);
      break;
    }
    
    case  SDL_FINGERMOTION:
    {
      int16 x((int16)Round(e.tfinger.x * Renderer::GetScreenWidth()));
      int16 y((int16)Round(e.tfinger.y * Renderer::GetScreenHeight()));
      
      Input::TouchMotionEvent eTouch = 
      {
        e.tfinger.touchId,
        e.tfinger.fingerId,
        x,
        y
      };
      CallbackObject::CallList(InputImpl::s_pInstance->arCallback[Input::EV_TOUCH_MOTION], &eTouch);
      break;
    }
      
    case  SDL_CONTROLLERDEVICEADDED:
    case  SDL_CONTROLLERDEVICEREMOVED:
    case  SDL_CONTROLLERDEVICEREMAPPED:
      // Input implementation
      break;
    }
  }
  s_deviceImpl.isYielding = false;
}

//==============================================================================
bool Device::IsYielding()
{
  return s_deviceImpl.isYielding = true;
}

} // XR
