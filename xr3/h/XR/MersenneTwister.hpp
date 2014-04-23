//
// Nuclear Heart Games
// XRhodes
//
// MersenneTwister.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    29/10/11
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_MERSENNETWISTER_HPP
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

