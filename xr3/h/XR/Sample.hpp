//
// Nuclear Heart Games
// XRhodes
//
// Sample.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    14/10/2011
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_SAMPLE_HPP
#define XR_SAMPLE_HPP

#include "ResManager.hpp"
#include "CrossObject.hpp"
#include "SampleFormat.hpp"

namespace XR
{

//==============================================================================
class Sample:  protected CrossObject
{
  XR_CROSS_OBJECT_DECL(Sample)
  
public:
  // general
  int   GetFormat() const;
  int   GetBufferSize() const;
  int   GetNumSamples() const;
  void* GetData() const;
  
  bool  Load(const char* pName);
};

typedef ResManager<Sample> SampleManager;

} // XR

#endif // XR_SAMPLE_HPP