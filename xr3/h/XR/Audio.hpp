//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_AUDIO_HPP
#define XR_AUDIO_HPP

#include "Sample.hpp"

namespace XR
{

//==============================================================================
class Audio
{
  XR_NONOBJECT_DECL(Audio)
  
public:
  //
  enum  ChannelID
  {
    CHANNEL_ANY = -1, // in params
    CHANNEL_ALL = -1, // in params
    INVALID_CHANNEL = -1  // in return
  };
  
  enum
  {
    INFINITY_TIMES = 0
  };
  
  enum  Panning
  {
    PAN_LEFT = 0x00,
    PAN_CENTRE = 0x7f,
    PAN_RIGHT = 0xff
  };
  
  // static
  static void Init();
  static void Exit();
  
  static int  GetNumChannels();
  static int  GetSampleRate();
  
  static void SetSampleRate(int rate);
  
  static int  Play(Sample& s, int times = 1, int channelHint = CHANNEL_ANY);
  static void PauseChannel(int channelId);
  static void ResumeChannel(int channelId);
  static void StopChannel(int channelId);
  
  static bool IsChannelPlaying(int channelId);
  static bool IsChannelPaused(int channelId);
  
  static int  GetChannelVolume(int channelId);
  static int  GetChannelPanning(int channelId);
  static int  GetChannelPitch(int channelId);

  static void SetChannelVolume(int channelId, int volume);
  static void SetChannelPanning(int channelId, int pan);
  static void SetChannelPitch(int channelId, int pitch);
  
  static void PauseAll();
  static void ResumeAll();
  static void StopAll();
};

} // XR

#endif // XR_AUDIO_HPP