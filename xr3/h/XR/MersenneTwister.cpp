//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "MersenneTwister.hpp"

namespace XR
{

//==============================================================================
MersenneTwister::MersenneTwister(unsigned int seed)
: m_state(),
  m_index(0)
{
  Seed(seed);
}

//==============================================================================
MersenneTwister::~MersenneTwister()
{}

//==============================================================================
void  MersenneTwister::Seed(unsigned int seed)
{
  m_index = 0;
  m_state[0] = seed;
  for (unsigned int i = 1; i < kSize; ++i)
  {
    const unsigned int &prev(m_state[i - 1]);
    m_state[i] = (1812433253UL * (prev ^ (prev >> 30)) + 1);
  }
  
  _Generate();
}

//==============================================================================
unsigned int MersenneTwister::Rand()
{
  unsigned int y = m_state[m_index];
  y ^= y >> 11;
  y ^= (y << 7) & 2636928640UL;
  y ^= (y << 15) & 4022730752UL;
  y ^= (y >> 18);

  ++m_index;
  if (m_index >= kSize)
  {
    m_index -= kSize;
    _Generate();
  }
  return y;
}

//==============================================================================
void  MersenneTwister::_Generate()
{
  for (unsigned int i = 0; i < kSize; ++i)
  {
    unsigned int y((m_state[m_index] & 0x01) +
      (m_state[(m_index + 1) % kSize] & 0x7fffffff));

    unsigned int &state(m_state[i]);
    state = m_state[(i + 397) % kSize] ^ (y >> 1);
    if ((y & 1) == 1)
    {
      state = state ^ 2567483615UL;
    }
  }
}

} //XR