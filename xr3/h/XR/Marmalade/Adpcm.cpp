//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "Adpcm.hpp"

namespace XR
{

//==============================================================================
static int32  s_arDiffTable[89 * 8];

//==============================================================================
const int16 Adpcm::karImaStepTable[89] =
{
  7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
  19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
  50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
  130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
  337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
  876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
  2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
  5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
  15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
};

const int16 Adpcm::karImaIndexTable[16] =
{
  -1, -1, -1, -1,  // +0 - +3, decrease the step size
  2, 4, 6, 8,     // +4 - +7, increase the step size
  -1, -1, -1, -1,  // -0 - -3, decrease the step size
  2, 4, 6, 8,     // -4 - -7, increase the step size
};

const int32* Adpcm::karImaDiffTable(s_arDiffTable);

//==============================================================================
void Adpcm::InitDiffTable()
{
  for(int i = 0; i < 89; ++i)
  {
    for(int j = 0; j < 8; ++j)
    {
      s_arDiffTable[i * 8 + j] = ((karImaStepTable[i] * j) >> 2) +
        (karImaStepTable[i] >> 3);
    }
  }
}

//==============================================================================
uint8 Adpcm::EncodeSample( int16 pcm, State& s )
{
  int   delta(pcm - s.prevValue);
  uint8 nibble((delta < 0) * 0x8);

  return nibble;
}

//==============================================================================
int Adpcm::Encode( int inSize, int16* pInBuffer, uint8* pOutBuffer )
{
  XR_ASSERT(Adpcm, inSize > 1);
  XR_ASSERT(Adpcm, pInBuffer != 0);
  XR_ASSERT(Adpcm, pOutBuffer != 0);
  
  inSize &= ~1;
  const int16*  pEnd(pInBuffer + inSize);
  
  //State s;

  return 0;
}

} // XR
