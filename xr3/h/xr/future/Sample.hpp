#ifndef XR_SAMPLE_HPP
#define XR_SAMPLE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================

#include "ResManager.hpp"
#include "CrossObject.hpp"
#include "SampleFormat.hpp"

namespace xr
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

} // xr

#endif // XR_SAMPLE_HPP
