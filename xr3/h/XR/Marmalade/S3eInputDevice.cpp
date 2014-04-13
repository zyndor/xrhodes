#include "S3eInputDevice.hpp"
#include "Timer.hpp"

namespace XR
{

static const float kAccelerometerNormal = 1.0f / 1000.0f;

int32 S3eInputDevice::OnPointerTouch( void* pSystem, void* pUser )
{
  s3ePointerEvent*  pEvent(static_cast<s3ePointerEvent*>(pSystem));
  S3eInputDevice*   pDevice(static_cast<S3eInputDevice*>(pUser));
  
  s3ePointerTouchEvent  ev;
  ev.m_TouchID = 0;
  ev.m_Pressed = pEvent->m_Pressed;
  ev.m_x = pEvent->m_x;
  ev.m_y = pEvent->m_y;
  
  return OnPointerMultiTouch(&ev, pUser);
}

int32 S3eInputDevice::OnPointerMotion( void* pSystem, void* pUser )
{
  s3ePointerMotionEvent*  pEvent(static_cast<s3ePointerMotionEvent*>(pSystem));
  
  s3ePointerTouchMotionEvent  ev;
  ev.m_TouchID = 0;
  ev.m_x = pEvent->m_x;
  ev.m_y = pEvent->m_y;
  
  return OnPointerMultiMotion(&ev, pUser);
}

int32 S3eInputDevice::OnPointerMultiTouch( void* pSystem, void* pUser )
{
  s3ePointerTouchEvent* pEvent(static_cast<s3ePointerTouchEvent*>(pSystem));
  S3eInputDevice*       pDevice(static_cast<S3eInputDevice*>(pUser));

  Input::Touch&  t(pDevice->m_arTouch[pEvent->m_TouchID]);

  if(!t.isPressed)
  {
    t.x0 = pEvent->m_x;
    t.y0 = pEvent->m_y;
    t.ustPressed = Timer::GetUST();
  }
  
  t.isPressed = pEvent->m_Pressed != 0;
  t.x = pEvent->m_x;
  t.y = pEvent->m_y;
  
  CallbackObject::List& l(pDevice->m_arlHandlers[EV_POINTER_ACTION]);
  for(CallbackObject::List::iterator i0(l.begin()), i1(l.end()); i0 != i1; ++i0)
  {
    (*i0->pCb)(&t, i0->pCbData);
  }

  return 0;
}

int32 S3eInputDevice::OnPointerMultiMotion( void* pSystem, void* pUser )
{
  s3ePointerTouchMotionEvent* pEvent(static_cast<s3ePointerTouchMotionEvent*>(pSystem));
  S3eInputDevice*             pDevice(static_cast<S3eInputDevice*>(pUser));

  Input::Touch& t(pDevice->m_arTouch[pEvent->m_TouchID]);
  if(t.isPressed)
  {
    t.x = pEvent->m_x;
    t.y = pEvent->m_y;
  }

  CallbackObject::List& l(pDevice->m_arlHandlers[EV_POINTER_MOTION]);
  for(CallbackObject::List::iterator i0(l.begin()), i1(l.end()); i0 != i1; ++i0)
  {
    (*i0->pCb)(&t, i0->pCbData);
  }

  return 0;
}

S3eInputDevice::S3eInputDevice()
: Device(),
  m_arTouch(),
  m_arButtonState(),
  m_numActiveTouches(0),
  m_arActiveTouchId()
{
  memset(m_arTouch, 0x00, sizeof(m_arTouch));
  memset(m_arActiveTouchId, 0x00, sizeof(m_arActiveTouchId));

  memset(m_arButton, 0x00, sizeof(m_arButton));
    
  m_arButton[Input::B_LEFT_1] = s3eKeyLeft;
  m_arButton[Input::B_RIGHT_1] = s3eKeyRight;
  m_arButton[Input::B_UP_1] = s3eKeyUp;
  m_arButton[Input::B_DOWN_1] = s3eKeyDown;

  m_arButton[Input::B_LEFT_2] = s3eKeyA;
  m_arButton[Input::B_RIGHT_2] = s3eKeyD;
  m_arButton[Input::B_UP_2] = s3eKeyW;
  m_arButton[Input::B_DOWN_2] = s3eKeyS;

  m_arButton[Input::B_LSHOULDER] = s3eKeyQ;

  m_arButton[Input::B_RSHOULDER] = s3eKeyE;
  
  m_arButton[Input::B_ACTION_1] = s3eKeyEnter;
  
  m_arButton[Input::B_OPTION_1] = s3eKeyF1;
  m_arButton[Input::B_OPTION_2] = s3eKeyF2;
  m_arButton[Input::B_OPTION_3] = s3eKeyF3;
  m_arButton[Input::B_OPTION_4] = s3eKeyF4;

  m_arButton[Input::B_SYSTEM] = s3eKeyEsc;

  memset(m_arButtonState, 0x00, sizeof(m_arButtonState));
  
  if(HasTouch())
  {
    if(GetMaxTouches() > 1)
    {
      s3ePointerRegister(S3E_POINTER_TOUCH_EVENT, OnPointerMultiTouch, this);
      s3ePointerRegister(S3E_POINTER_TOUCH_MOTION_EVENT, OnPointerMultiMotion, this);
    }
    else
    {
      s3ePointerRegister(S3E_POINTER_BUTTON_EVENT, OnPointerTouch, this);
      s3ePointerRegister(S3E_POINTER_MOTION_EVENT, OnPointerMotion, this);
    }
  }
}

S3eInputDevice::~S3eInputDevice()
{
  s3ePointerUnRegister(S3E_POINTER_BUTTON_EVENT, OnPointerTouch);
  s3ePointerUnRegister(S3E_POINTER_MOTION_EVENT, OnPointerMotion);
  s3ePointerUnRegister(S3E_POINTER_TOUCH_EVENT, OnPointerMultiTouch);
  s3ePointerUnRegister(S3E_POINTER_TOUCH_MOTION_EVENT, OnPointerMultiMotion);
  StopAccelerometer();
}

const int8* S3eInputDevice::GetName() const
{
  return (const int8*)"S3eInputDevice";
}

bool S3eInputDevice::HasButtons() const
{
  return true;
}

bool S3eInputDevice::HasTouch() const
{
  return s3ePointerGetInt(S3E_POINTER_AVAILABLE) != 0;  // stub
}

bool S3eInputDevice::HasAccelerometer() const
{
  return s3eAccelerometerGetInt(S3E_ACCELEROMETER_AVAILABLE) != 0;  // stub
}

void S3eInputDevice::StartAccelerometer()
{
  s3eAccelerometerStart();
}

void S3eInputDevice::StopAccelerometer()
{
  s3eAccelerometerStop();
}

int32 S3eInputDevice::GetMaxAxes() const
{
  return 0;  // stub
}

int32 S3eInputDevice::GetMaxTouches() const
{
  return HasTouch() ? (s3ePointerGetInt(S3E_POINTER_MULTI_TOUCH_AVAILABLE) ? S3E_POINTER_TOUCH_MAX : 1) : 0;
}

int32 S3eInputDevice::GetButtonState( int32 id ) const
{
  return m_arButtonState[id];
}

float S3eInputDevice::GetAxisState( int32 id ) const
{
  return .0f;
}

const Input::Touch* S3eInputDevice::GetTouchState( int32 id ) const
{
  return &(m_arTouch[id]);
}

Vector3 S3eInputDevice::GetAccelerometerState() const
{
  return Vector3(s3eAccelerometerGetX() * kAccelerometerNormal,
    s3eAccelerometerGetY() * kAccelerometerNormal,
    s3eAccelerometerGetZ() * kAccelerometerNormal); // stub
}

void S3eInputDevice::Update()
{
  for(int32 i = 0; i < Input::kMaxButtons; ++i)
  {
    int32 keyState(s3eKeyboardGetState(m_arButton[i]));
    int32 buttonState(0);
    if(keyState & S3E_KEY_STATE_UP)
    {
      buttonState |= Input::BS_UP;
    }
    if(keyState & S3E_KEY_STATE_DOWN)
    {
      buttonState |= Input::BS_DOWN;
    }
    if(keyState & S3E_KEY_STATE_PRESSED)
    {
      buttonState |= Input::BS_PRESSED;
    }
    if(keyState & S3E_KEY_STATE_RELEASED)
    {
      buttonState |= Input::BS_RELEASED;
    }
    m_arButtonState[i] = buttonState;
  }
  
  int32 n(0);
  for(int32 i = 0; i < S3E_POINTER_TOUCH_MAX; ++i)
  {
    if(m_arTouch[i].isPressed)
    {
      m_arActiveTouchId[n] = i;
      ++n;
    }
  }
  m_numActiveTouches = n;
}

void S3eInputDevice::SetButton( int32 id, s3eKey code )
{
  XR_ASSERT(S3eInputDevice, id >= 0);
  XR_ASSERT(S3eInputDevice, id < Input::kMaxButtons);
  
  m_arButton[id] = code;
}

int32 S3eInputDevice::GetNumTouchWithin( int16 x, int16 y, int16 w, int16 h ) const
{
  int32 numTouchWithin(0);
  for(int32 i = 0; i < S3E_POINTER_TOUCH_MAX; ++i)
  {
    if(m_arTouch[i].isPressed && m_arTouch[i].x >= x && m_arTouch[i].x < x + w &&
      m_arTouch[i].y >= y && m_arTouch[i].y < y + h)
    {
      ++numTouchWithin;
    }
  }
  return numTouchWithin;
}

int32 S3eInputDevice::GetNumGesturesWithin( int16 x, int16 y, int16 w, int16 h ) const
{
  int32 numGesturesWithin(0);
  for(int32 i = 0; i < S3E_POINTER_TOUCH_MAX; ++i)
  {
    if(m_arTouch[i].isPressed && m_arTouch[i].x >= x && m_arTouch[i].x < x + w &&
      m_arTouch[i].y >= y && m_arTouch[i].y < y + h &&
      m_arTouch[i].x0 >= x && m_arTouch[i].x0 < x + w &&
      m_arTouch[i].y0 >= y && m_arTouch[i].y0 < y + h)
    {
      ++numGesturesWithin;
    }
  }
  return numGesturesWithin;
}

int32 S3eInputDevice::GetNumTouchPressed() const
{
  return m_numActiveTouches;
}

const int32* S3eInputDevice::GetActiveTouchIds() const
{
  return m_arActiveTouchId;
}

bool S3eInputDevice::RegisterHandler( Event ev, Callback pCb, void* pCbData )
{
  XR_ASSERT(Renderer, ev < kMaxEvent);
  XR_ASSERT(S3eInputDevice, pCb != 0);
  for(CallbackObject::List::iterator i0(m_arlHandlers[ev].begin()),
    i1(m_arlHandlers[ev].end()); i0 != i1; ++i0)
  {
    if(i0->pCb == pCb)
    {
      return false;
    }
  }

  CallbackObject cb = { pCb, pCbData };
  m_arlHandlers[ev].push_back(cb);
  return true;
}

bool S3eInputDevice::UnregisterHandler( Event ev, Callback pCb )
{
  XR_ASSERT(Renderer, ev < Input::Device::kMaxEvent);
  XR_ASSERT(S3eInputDevice, pCb != 0);
  for(CallbackObject::List::iterator i0(m_arlHandlers[ev].begin()),
    i1(m_arlHandlers[ev].end()); i0 != i1; ++i0)
  {
    if(i0->pCb == pCb)
    {
      m_arlHandlers[ev].erase(i0);
      return true;
    }
  }
  return false;
}

} // XR
