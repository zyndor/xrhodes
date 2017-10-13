//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/Audio.hpp"
#include "XR/Device.hpp"
#include "XR/debug.hpp"
#include <SDL_mixer.h>
#include <string.h>

namespace XR
{

static struct  
{
  int     numChannels;
  int     sampleRate;
  uint8_t*  parPanning;
} s_audioImpl;

//==============================================================================
static int OnChannelGenAudioAdpcm(void* pSystem, void* pUser)
{
  return 0;
}

//==============================================================================
static int  OnChannelEndSample(void* pSystem, void* pUser)
{
  return 0;
}

//==============================================================================
void Audio::Init()
{
  XR_ASSERTMSG(Audio, true, ("Already initialised!"));

  int freq(MIX_DEFAULT_FREQUENCY);
  int chunkSize(4096);
  int channels(16);
  
  // todo - attempt to read from config
  
  int result(Mix_OpenAudio(freq, MIX_DEFAULT_FORMAT, 2, chunkSize));
  if (result == -1)
  {
    XR_ERROR(("Error initialising Audio."));
  }
  
  s_audioImpl.numChannels = Mix_AllocateChannels(channels);
  s_audioImpl.parPanning = new uint8_t[channels];
}

//==============================================================================
void Audio::Exit()
{
  StopAll();
  
  delete[] s_audioImpl.parPanning;
  memset(&s_audioImpl, 0x00, sizeof(s_audioImpl));
  
  Mix_CloseAudio();
}

//==============================================================================
int Audio::GetNumChannels()
{
  return s_audioImpl.numChannels;
}

//==============================================================================
int Audio::Play( Sample& s, int times /*= 1*/, int channelHint /*= CHANNEL_ANY*/ )
{
  XR_ASSERT(Audio, times >= INFINITY_TIMES);
  XR_ASSERT(Audio, channelHint >= CHANNEL_ANY);
  XR_ASSERT(Audio, channelHint < s_audioImpl.numChannels);
  channelHint = Mix_PlayChannelTimed(channelHint,
    static_cast<Mix_Chunk*>(s.GetImpl()), times - 1, -1);
  
  return channelHint;
}

//==============================================================================
void Audio::PauseChannel( int channelId )
{
  XR_ASSERT(Audio, channelId >= 0);
  XR_ASSERT(Audio, channelId < s_audioImpl.numChannels);
  if (Mix_Paused(channelId) == 0)
  {
    Mix_Pause(channelId);
  }
}

//==============================================================================
void Audio::ResumeChannel( int channelId )
{
  XR_ASSERT(Audio, channelId >= 0);
  XR_ASSERT(Audio, channelId < s_audioImpl.numChannels);
  if (Mix_Paused(channelId) != 0)
  {
    Mix_Pause(channelId);
  }
}

//==============================================================================
void Audio::StopChannel( int channelId )
{
  XR_ASSERT(Audio, channelId >= 0);
  XR_ASSERT(Audio, channelId < s_audioImpl.numChannels);
  Mix_HaltChannel(channelId);
}

//==============================================================================
void Audio::PauseAll()
{
  Mix_Pause(CHANNEL_ALL);
}

//==============================================================================
void Audio::ResumeAll()
{
  Mix_Resume(CHANNEL_ALL);
}

//==============================================================================
void Audio::StopAll()
{
  Mix_HaltChannel(CHANNEL_ALL);
}

//==============================================================================
bool Audio::IsChannelPlaying( int channelId )
{
  XR_ASSERT(Audio, channelId >= 0);
  XR_ASSERT(Audio, channelId < s_audioImpl.numChannels);
  return Mix_Playing(channelId) != 0;
}

//==============================================================================
bool Audio::IsChannelPaused( int channelId )
{
  XR_ASSERT(Audio, channelId >= 0);
  XR_ASSERT(Audio, channelId < s_audioImpl.numChannels);
  return Mix_Paused(channelId) != 0;
}

//==============================================================================
int Audio::GetChannelVolume( int channelId )
{
  XR_ASSERT(Audio, channelId >= 0);
  XR_ASSERT(Audio, channelId < s_audioImpl.numChannels);
  return Mix_Volume(channelId, -1);
}

//==============================================================================
int Audio::GetChannelPanning( int channelId )
{
  XR_ASSERT(Audio, channelId >= 0);
  XR_ASSERT(Audio, channelId < s_audioImpl.numChannels);
  return s_audioImpl.parPanning[channelId];
}

//==============================================================================
int Audio::GetChannelPitch( int channelId )
{
  XR_ASSERT(Audio, channelId >= 0);
  XR_ASSERT(Audio, channelId < s_audioImpl.numChannels);
  return 1; // todo
}

//==============================================================================
void Audio::SetChannelVolume( int channelId, int volume )
{
  XR_ASSERT(Audio, channelId >= 0);
  XR_ASSERT(Audio, channelId < s_audioImpl.numChannels);
  Mix_Volume(channelId, volume);
}

//==============================================================================
void Audio::SetChannelPanning( int channelId, int pan )
{
  XR_ASSERT(Audio, channelId >= 0);
  XR_ASSERT(Audio, channelId < s_audioImpl.numChannels);
  XR_ASSERT(Audio, pan >= PAN_LEFT);
  XR_ASSERT(Audio, pan <= PAN_RIGHT);
  Mix_SetPanning(channelId, PAN_RIGHT - pan, pan);
  s_audioImpl.parPanning[channelId] = pan;
}

//==============================================================================
void Audio::SetChannelPitch( int channelId, int pitch )
{
  XR_ASSERT(Audio, channelId >= 0);
  XR_ASSERT(Audio, channelId < s_audioImpl.numChannels);
  // todo
}

//==============================================================================
int Audio::GetSampleRate()
{
  return s_audioImpl.sampleRate;
}

} // XR
