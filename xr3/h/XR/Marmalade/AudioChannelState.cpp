#include "AudioChannelState.hpp"
#include <s3eSound.h>

namespace XR
{

//==============================================================================
void AudioChannelState::Start( SampleData* p )
{
  XR_ASSERT(AudioChannelState, pSound == 0);
  pSound = p;
  pPos = p->parSamples;
  pEnd = pPos + p->numSamples;
  pLast = pPos;
  state = *(Adpcm::State*)pPos;
  nibbles = pSound->samplesPerBlock - 1;
  rateError = 0;
}

//==============================================================================
void AudioChannelState::Restart()
{
  SampleData*  p(pSound);
  pSound = 0;  
  Start(p);
}

//==============================================================================
void AudioChannelState::Stop()
{
  pSound = 0;

  pPos = 0;
  pEnd = 0;
  state.prevValue = 0;
  state.iStep = 0;
  state.stepSize = Adpcm::karImaStepTable[state.iStep];
  nibbles = 0;
  rateError = 0;
}

//==============================================================================
enum
{
  FIX_POINT = 12,
  FIX_ONE = 1 << FIX_POINT
}; // 256-base

#define FIX_MULT(a, b) (((a) * (b)) >> FIX_POINT)
#define FIX_DIV(a, b) (((a) << FIX_POINT) / (b))

int16 ClipToInt16(int32 val)
{
  if (((val - std::numeric_limits<int16>::min()) &
    ~std::numeric_limits<uint16>::max()) != 0)
  {
    bool  over(val > std::numeric_limits<int16>::max());
    bool  under(val < std::numeric_limits<int16>::min());
    XR_ASSERT(ClipToInt16, over ^ under);
    val = over * std::numeric_limits<int16>::max() + under *
      std::numeric_limits<int16>::min();
  }

  return static_cast<int16>(val);
}

int32 AudioChannelState::GenerateAdpcm( void* pSystem )
{
  s3eSoundGenAudioInfo* pInfo(static_cast<s3eSoundGenAudioInfo*>(pSystem));
  
  bool    stereo(pInfo->m_Stereo != 0);
  bool    mix(pInfo->m_Mix != 0);

  int     volume(s3eSoundChannelGetInt(pInfo->m_Channel, S3E_CHANNEL_VOLUME));
  int     volRight(s3eSoundChannelGetInt(pInfo->m_Channel, S3E_CHANNEL_USERVAR));
  int     volLeft(0xff - volRight);
  
  volLeft *= volume;
  volRight *= volume;
  
  volLeft >>= 8;
  volRight >>= 8;

  int     channelRate(s3eSoundChannelGetInt(pInfo->m_Channel, S3E_CHANNEL_RATE));
  int     mixRate(s3eSoundGetInt(S3E_SOUND_OUTPUT_FREQ));
  int     relRate(FIX_DIV(channelRate, mixRate));
  
  int     numWritten(0);
  int     toWrite(pInfo->m_NumSamples);
  int16*  pOutBuffer(pInfo->m_Target);
  
  AudioChannelState   temp(*this);
  int                 sCurrent(temp.state.prevValue);
  
  bool  isFinished(numWritten >= toWrite);
  bool  skipInit((numWritten == 0) && (temp.nibbles & 1));
  XR_ASSERT(AudioChannelState, temp.nibbles > 0);
  while(!isFinished)
  {
    int nibble(*temp.pPos);
    int i = skipInit;
    
    while(i < 2)
    {
      if(skipInit)
      {
        nibble >>= 4;
        skipInit = false;
      }
      else
      {
        int delta(nibble & 0x7);  // b0111
        int sign(nibble & 0x8); // b1000
        int diff(Adpcm::karImaDiffTable[(temp.state.iStep << 3) + delta]);

        sCurrent = (sign ? temp.state.prevValue - diff :
          temp.state.prevValue + diff);
        //s1 = ClipToInt16(s1);

        temp.state.iStep += Adpcm::karImaIndexTable[delta];
        if(temp.state.iStep < 0)
        {
          temp.state.iStep = 0;
        }
        else if(temp.state.iStep > 88)
        {
          temp.state.iStep = 88;
        }

        temp.rateError += FIX_ONE;
      }
      
      int sDiff(ClipToInt16(sCurrent - temp.state.prevValue));
      if(stereo)
      {
        while(temp.rateError >= relRate)
        {
          temp.rateError -= relRate;

          int32 smp(FIX_MULT(sDiff, relRate) + temp.state.prevValue);
          pOutBuffer[0] = ClipToInt16(pOutBuffer[0] + ((smp * volLeft) >> 8));
          pOutBuffer[1] = ClipToInt16(pOutBuffer[1] + ((smp * volRight) >> 8));

          pOutBuffer += 2;

          ++numWritten;
          isFinished = numWritten >= toWrite;
          if(isFinished)
          {
            break;
          }

          // if not mixing, clear samples
          pOutBuffer[0] *= mix;
          pOutBuffer[1] *= mix;
        }
      }
      else
      {
        while(temp.rateError >= relRate)
        {
          temp.rateError -= relRate;

          int32 smp(FIX_MULT(sDiff, relRate) + temp.state.prevValue);
          *pOutBuffer = ClipToInt16(*pOutBuffer + ((smp * volume) >> 8));

          ++pOutBuffer;

          ++numWritten;
          isFinished = numWritten >= toWrite;
          if(isFinished)
          {
            break;
          }

          // if not mixing, clear samples
          *pOutBuffer *= mix;
        }
      }
      
      if(isFinished)
      {
        break;
      }

      temp.state.prevValue = sCurrent;
      nibble >>= 4;
      --temp.nibbles;
      ++i;
    }

    if(!isFinished)
    {
      ++temp.pPos;
      if(temp.pPos >= temp.pEnd)
      {
        pInfo->m_EndSample = 1;
        break;
      }
    }
    
    if(temp.nibbles <= 0)
    {
      int blockAlign(pSound->blockAlign);
      temp.pLast += blockAlign;
      temp.nibbles = pSound->samplesPerBlock - 1;

      if(temp.pLast > temp.pEnd)
      {
        temp.nibbles -= (temp.pLast - temp.pEnd) * 2;
      }

      temp.pPos = temp.pLast + sizeof(Adpcm::State);
      temp.state = *((Adpcm::State*)temp.pLast);
    }
  }
  
  *this = temp;
  
  return numWritten;
}

} // XR
