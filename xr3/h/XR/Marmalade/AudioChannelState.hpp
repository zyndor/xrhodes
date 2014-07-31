//
// Nuclear Heart Games
// XRhodes
//
// AudioChannelState.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    15/10/2013
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_AUDIOCHANNELSTATE_HPP
#define XR_AUDIOCHANNELSTATE_HPP

#include "SampleData.hpp"
#include "Adpcm.hpp"

namespace XR
{

//==============================================================================
class AudioChannelState 
{
public:
  // static
  static void   InitAdpcm();
  
  // data
  SampleData*   pSound;

  uint8*        pPos;
  uint8*        pEnd;
  uint8*        pLast;

  Adpcm::State  state;

  int           nibbles;
  int           rateError;
  
  // general
  void Start(SampleData* p);
  void Restart();
  void Stop();
  
  int32 GenerateAdpcm(void* pSystem);
};

} // XR

#endif // XR_AUDIOCHANNELSTATE_HPP