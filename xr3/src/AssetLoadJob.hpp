#ifndef XR_ASSETLOADJOB_HPP
#define XR_ASSETLOADJOB_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Asset.hpp"
#include "xr/threading/Worker.hpp"

namespace xr
{

//==============================================================================
class AssetLoadJob : public Worker::Job
{
public:
  // data
  Asset::Ptr asset;

  // structors
  AssetLoadJob(File::Handle hFile, size_t size, Asset::Ptr const& a);
  ~AssetLoadJob();

  // general
  virtual void Start() override;

  virtual bool Process() override;

  bool ProcessData();

private:
  // data
  File::Handle          mHFile = nullptr;
  std::vector<uint8_t>  mData;
  uint8_t*              mNextWrite = nullptr;

  void CloseHandle();
};

}
#endif //XR_ASSETLOADJOB_HPP
