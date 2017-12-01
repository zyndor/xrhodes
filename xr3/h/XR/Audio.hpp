#ifndef XR_AUDIO_HPP
#define XR_AUDIO_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include "Sample.hpp"
#include "XR/fundamentals.hpp"

namespace XR
{

//==============================================================================
///@brief Subsystem responsible for the playback of sound effects and music.
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
  
  ///@return The number of channels used for mixing.
  static int  GetNumChannels();

  ///@return The sample rate of the mixing, in Hz.
  static int  GetSampleRate();
  
  ///@brief Sets the sample rate of mixing, in Hz.
  static void SetSampleRate(int rate);
  
  ///@brief Plays a Sample @a s, @a times times, using the channel with
  /// the given id, @a channelHint (by default, this will be the first
  /// channel that has its playback stopped).
  ///@return The id of the channel that the Sample is playing on.
  static int  Play(Sample& s, int times = 1, int channelHint = CHANNEL_ANY);
  
  ///@brief Pauses playback on the given channel. Does nothing if the channel
  /// wasn't playing in the first place.
  static void PauseChannel(int channelId);

  ///@brief Resumes playback on the given channel. Does nothing if the channel
  /// wasn't paused in the first place.
  static void ResumeChannel(int channelId);

  ///@brief Stops playback on the given channel. Does nothing if the channel
  /// was already stopped.
  static void StopChannel(int channelId);
  
  ///@brief Pauses playback on every channel.
  static void PauseAll();

  ///@brief Resumes playback on every channel that was paused.
  static void ResumeAll();

  ///@brief Stops playback on every channel.
  static void StopAll();

  ///@return Whether the channel is playing a sample.
  static bool IsChannelPlaying(int channelId);

  ///@return Whether the channel currently has its playback paused.
  static bool IsChannelPaused(int channelId);
  
  ///@return The playback volume of the given channel.
  static int  GetChannelVolume(int channelId);

  ///@return The playback volume of the given channel.
  static int  GetChannelPanning(int channelId);

  ///@return The pitch of the given channel.
  static int  GetChannelPitch(int channelId);

  ///@brief Sets the playback volume of the channel.
  static void SetChannelVolume(int channelId, int volume);

  ///@brief Sets the panning of the channel.
  static void SetChannelPanning(int channelId, int pan);

  ///@brief Sets the pitch of the channel.
  static void SetChannelPitch(int channelId, int pitch);
};

} // XR

#endif // XR_AUDIO_HPP