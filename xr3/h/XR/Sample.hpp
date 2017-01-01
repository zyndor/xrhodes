//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_SAMPLE_HPP
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