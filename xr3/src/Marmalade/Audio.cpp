//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "Audio.hpp"
#include "AudioChannelState.hpp"
#include <XR/debug.hpp>
#include <s3eSound.h>

namespace XR
{

//==============================================================================
struct  AudioImpl
{
  int                 numChannels;
  AudioChannelState*  parChannelState;
};

static AudioImpl*  s_pAudio(0);

//==============================================================================
static int OnChannelGenAudioAdpcm(void* pSystem, void* pUser)
{
  AudioChannelState*     pChannel(static_cast<AudioChannelState*>(pUser));
  return pChannel->GenerateAdpcm(pSystem);
}

//==============================================================================
static int  OnChannelEndSample(void* pSystem, void* pUser)
{
  s3eSoundEndSampleInfo*  pInfo(static_cast<s3eSoundEndSampleInfo*>(pSystem));
  AudioChannelState*       pChannel(static_cast<AudioChannelState*>(pUser));
  
  int result(pInfo->m_RepsRemaining != 0);
  if(result)
  {
    pChannel->Restart();
  }
  else
  {
    pChannel->Stop();
  }
  return result;
}

//==============================================================================
void Audio::Init()
{
  XR_ASSERTMSG(Audio, s_pAudio == 0, ("Already initialised!"));
  Adpcm::InitDiffTable();

  s_pAudio = new AudioImpl();
  s_pAudio->numChannels = s3eSoundGetInt(S3E_SOUND_NUM_CHANNELS);
  s_pAudio->parChannelState = new AudioChannelState[s_pAudio->numChannels];
  memset(s_pAudio->parChannelState, 0x00, sizeof(AudioChannelState) *
    s_pAudio->numChannels);
  
  XR_TRACE(Audio, ("number of channels: %d", s_pAudio->numChannels));
  XR_TRACE(Audio, ("sample rate: %d", s3eSoundGetInt(S3E_SOUND_OUTPUT_FREQ)));
  XR_TRACE(Audio, ("default volume: %d", s3eSoundGetInt(S3E_SOUND_VOLUME_DEFAULT)));
  XR_TRACE(Audio, ("current volume: %d", s3eSoundGetInt(S3E_SOUND_VOLUME)));
  
  for(int i = 0; i < s_pAudio->numChannels; ++i)
  {
    SetChannelPanning(i, PAN_CENTRE);
  }
}

//==============================================================================
void Audio::Exit()
{
  XR_ASSERTMSG(Audio, s_pAudio != 0, ("Already exited!"));
  StopAll();
  
  delete[] s_pAudio->parChannelState;
  delete s_pAudio;
  s_pAudio = 0;
}

//==============================================================================
int Audio::GetNumChannels()
{
  return s_pAudio->numChannels;
}

//==============================================================================
int Audio::Play( Sample& s, int times, int channelHint)
{
  XR_ASSERT(Audio, channelHint >= CHANNEL_ANY);
  XR_ASSERT(Audio, channelHint < s_pAudio->numChannels);
  if(channelHint == CHANNEL_ANY)
  {
    channelHint = s3eSoundGetFreeChannel();
  }
  
  if(IsChannelPlaying(channelHint))
  {
    StopChannel(channelHint);
  }
  
  AudioChannelState* pChannel(&s_pAudio->parChannelState[channelHint]);
  
  SampleData*  pSoundData(static_cast<SampleData*>(s.GetImpl()));
  pChannel->Start(pSoundData);
  
  int result;
  result = s3eSoundChannelSetInt(channelHint, S3E_CHANNEL_RATE, pSoundData->sampleRate);
  
  result += s3eSoundChannelRegister(channelHint, S3E_CHANNEL_GEN_AUDIO,
    OnChannelGenAudioAdpcm, pChannel);

  if(s3eSoundGetInt(S3E_SOUND_STEREO_ENABLED))
  {
    result += s3eSoundChannelRegister(channelHint, S3E_CHANNEL_GEN_AUDIO_STEREO,
      OnChannelGenAudioAdpcm, pChannel);
  }

  result += s3eSoundChannelRegister(channelHint, S3E_CHANNEL_END_SAMPLE,
    OnChannelEndSample, pChannel);

  result += s3eSoundChannelPlay(channelHint, (int16*)pSoundData->parSamples,
    pSoundData->numSamples, times, 0);  // stub
    
  return channelHint;
}

//==============================================================================
void Audio::PauseChannel( int channelId )
{
  XR_ASSERT(Audio, channelId >= 0);
  XR_ASSERT(Audio, channelId < s_pAudio->numChannels);
  s3eSoundChannelPause(channelId);
}

//==============================================================================
void Audio::ResumeChannel( int channelId )
{
  XR_ASSERT(Audio, channelId >= 0);
  XR_ASSERT(Audio, channelId < s_pAudio->numChannels);
  s3eSoundChannelResume(channelId);
}

//==============================================================================
void Audio::StopChannel( int channelId )
{
  XR_ASSERT(Audio, channelId >= 0);
  XR_ASSERT(Audio, channelId < s_pAudio->numChannels);
  s3eSoundChannelStop(channelId);
}

//==============================================================================
bool Audio::IsChannelPlaying( int channelId )
{
  XR_ASSERT(Audio, channelId >= 0);
  XR_ASSERT(Audio, channelId < s_pAudio->numChannels);
  return s3eSoundChannelGetInt(channelId, S3E_CHANNEL_STATUS) != 0;
}

//==============================================================================
bool Audio::IsChannelPaused( int channelId )
{
  XR_ASSERT(Audio, channelId >= 0);
  XR_ASSERT(Audio, channelId < s_pAudio->numChannels);
  return s3eSoundChannelGetInt(channelId, S3E_CHANNEL_PAUSED) != 0;
}

//==============================================================================
int Audio::GetChannelVolume( int channelId )
{
  XR_ASSERT(Audio, channelId >= 0);
  XR_ASSERT(Audio, channelId < s_pAudio->numChannels);
  return s3eSoundChannelGetInt(channelId, S3E_CHANNEL_VOLUME);
}

//==============================================================================
int Audio::GetChannelPanning( int channelId )
{
  XR_ASSERT(Audio, channelId >= 0);
  XR_ASSERT(Audio, channelId < s_pAudio->numChannels);
  return s3eSoundChannelGetInt(channelId, S3E_CHANNEL_USERVAR);
}

//==============================================================================
int Audio::GetChannelPitch( int channelId )
{
  XR_ASSERT(Audio, channelId >= 0);
  XR_ASSERT(Audio, channelId < s_pAudio->numChannels);
  return s3eSoundChannelGetInt(channelId, S3E_CHANNEL_PITCH);
}

//==============================================================================
void Audio::SetChannelVolume( int channelId, int volume )
{
  XR_ASSERT(Audio, channelId >= 0);
  XR_ASSERT(Audio, channelId < s_pAudio->numChannels);
  s3eSoundChannelSetInt(channelId, S3E_CHANNEL_VOLUME, volume);
}

//==============================================================================
void Audio::SetChannelPanning( int channelId, int pan )
{
  XR_ASSERT(Audio, channelId >= 0);
  XR_ASSERT(Audio, channelId < s_pAudio->numChannels);
  XR_ASSERT(Audio, pan >= PAN_LEFT);
  XR_ASSERT(Audio, pan <= PAN_RIGHT);
  s3eSoundChannelSetInt(channelId, S3E_CHANNEL_USERVAR, pan);
}

//==============================================================================
void Audio::SetChannelPitch( int channelId, int pitch )
{
  XR_ASSERT(Audio, channelId >= 0);
  XR_ASSERT(Audio, channelId < s_pAudio->numChannels);
  s3eSoundChannelSetInt(channelId, S3E_CHANNEL_PITCH, pitch);
}

//==============================================================================
void Audio::PauseAll()
{
  s3eSoundPauseAllChannels();
}

//==============================================================================
void Audio::ResumeAll()
{
  s3eSoundResumeAllChannels();
}

//==============================================================================
void Audio::StopAll()
{
  s3eSoundStopAllChannels();
}

//==============================================================================
int Audio::GetSampleRate()
{
  return s3eSoundGetInt(S3E_SOUND_OUTPUT_FREQ);
}

//==============================================================================
void Audio::SetSampleRate( int rate )
{
  s3eSoundSetInt(S3E_SOUND_OUTPUT_FREQ, rate);
}

} // XR
