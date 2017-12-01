#ifndef XR_AUDIOCHANNELSTATE_HPP
#define XR_AUDIOCHANNELSTATE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

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