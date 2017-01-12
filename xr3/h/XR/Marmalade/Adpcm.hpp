//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_ADPCM_HPP
#define XR_ADPCM_HPP

#include "fundamentals.hpp"
#include <cstdint>

namespace XR
{

//==============================================================================
class Adpcm
{
  XR_NONOBJECT_DECL(Adpcm)
  
public:
  // types
  struct  State
  {
    int16   prevValue;
    int8    iStep;
    int16   stepSize;
  };

  // static
  static const int16  karImaStepTable[89];
  static const int16  karImaIndexTable[16];
  
  static const int32* karImaDiffTable;
  
  static void InitDiffTable();

  static uint8 EncodeSample(int16 pcm, State& s);
  
  static int Encode(int inSize, int16* pInBuffer, uint8* pOutBuffer);
};

} // XR

#endif // XR_ADPCM_HPP