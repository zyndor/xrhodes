/********************************************************************
	created:  2013/10/04
	filename: S3eInputDevice.h
	author:   Gyorgy Straub
	copyright (c) 2013. All rights reserved.
	
	purpose:  
*********************************************************************/
#ifndef XR_S3EINPUTDEVICE_HPP
#define XR_S3EINPUTDEVICE_HPP

#include <s3e.h>
#include "Input.hpp"

namespace XR
{

class S3eInputDevice: public Input::Device
{
  XR_NONCOPY_DECL(S3eInputDevice)
  
public:
  // structors
  S3eInputDevice();
  virtual ~S3eInputDevice();
  
  // general
  virtual const int8*         GetName() const;

  virtual bool                HasButtons() const;
  virtual bool                HasTouch() const;
  virtual bool                HasAccelerometer() const;

  virtual void                StartAccelerometer();
  virtual void                StopAccelerometer();

  virtual int32               GetMaxAxes() const;
  virtual int32               GetMaxTouches() const;

  virtual int32               GetButtonState( int32 id ) const;
  virtual float               GetAxisState( int32 id ) const;
  virtual const Input::Touch* GetTouchState( int32 id ) const;
  virtual Vector3             GetAccelerometerState() const;
  
  virtual int32               GetNumTouchWithin( int16 x, int16 y, int16 w, int16 h ) const;
  virtual int32               GetNumGesturesWithin( int16 x, int16 y, int16 w, int16 h ) const;

  virtual int32               GetNumTouchPressed() const;

  virtual const int32*        GetActiveTouchIds() const;

  virtual void                Update();

  virtual bool                RegisterHandler( Event ev, Callback pCb,
                                void* pCbData );
  virtual bool                UnregisterHandler( Event ev, Callback pCb );
  
  void SetButton( int32 id, s3eKey code );

protected:
  // static
  static int32  OnPointerTouch(void* pSystem, void* pUser);
  static int32  OnPointerMotion(void* pSystem, void* pUser);
  static int32  OnPointerMultiTouch(void* pSystem, void* pUser);
  static int32  OnPointerMultiMotion(void* pSystem, void* pUser);

  // data
  Input::Touch          m_arTouch[S3E_POINTER_TOUCH_MAX];
  s3eKey                m_arButton[Input::kMaxButtons];
  int32                 m_arButtonState[Input::kMaxButtons];
  int32                 m_numActiveTouches;
  int32                 m_arActiveTouchId[S3E_POINTER_TOUCH_MAX];

  CallbackObject::List  m_arlHandlers[kMaxEvent];
};

} // XR

#endif // XR_S3EINPUTDEVICE_HPP