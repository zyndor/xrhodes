//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_MERSENNETWISTER_HPP
#define	XR_MERSENNETWISTER_HPP

namespace XR
{

//==============================================================================
class MersenneTwister
{
public:
  // structors
  explicit MersenneTwister(unsigned int seed = 0);
  ~MersenneTwister();
  
  // general
  void  Seed(unsigned int seed);
  
  unsigned int  Rand();
  int           RandSign();
  
private:
  // static
  static const unsigned int  kSize = 624;
  
  // internal
  void  _Generate();
  
  // data
  unsigned int m_state[kSize];
  unsigned int m_index;
};

//==============================================================================
// implementation
//==============================================================================
inline
int MersenneTwister::RandSign()
{
  return ((Rand() & 1) << 1) - 1;
}

} // XR

#endif	// XR_MERSENNETWISTER_HPP

