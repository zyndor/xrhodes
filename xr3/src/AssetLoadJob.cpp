//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "AssetLoadJob.hpp"

namespace xr
{

//==============================================================================
const size_t kChunkSizeBytes = XR_KBYTES(16);

//==============================================================================
AssetLoadJob::AssetLoadJob(File::Handle hFile, size_t size, Asset::Ptr const & a)
: asset(a),
  mHFile(hFile),
  mData(size)
{}

//==============================================================================
AssetLoadJob::~AssetLoadJob()
{
  CloseHandle();
}

//==============================================================================
void AssetLoadJob::Start()
{
  mNextWrite = mData.data();
}

//==============================================================================
bool AssetLoadJob::Process()
{
  const auto progress = mNextWrite - mData.data();
  const size_t nextChunkSize = std::min(kChunkSizeBytes, mData.size() - progress);
  const size_t readSize = File::Read(mHFile, 1, nextChunkSize, mNextWrite);
  bool done = false;
  if (readSize != nextChunkSize)
  {
    XR_TRACE(Asset::Manager, ("Failed to read %d bytes of asset '%s' @ %d of %d bytes",
      nextChunkSize, asset->GetDescriptor().ToPath().c_str(), progress,
      mData.size()));
    asset->FlagError();
    done = true;
  }
  else
  {
    mNextWrite += readSize;

    if (progress + readSize == mData.size())
    {
      asset->OverrideFlags(Asset::LoadingFlag, Asset::ProcessingFlag);
      done = true;
    }
  }

  return done;
}

//==============================================================================
bool AssetLoadJob::ProcessData()
{
  return asset->ProcessData({ mData.size(), mData.data() });
}

//==============================================================================
void AssetLoadJob::CloseHandle()
{
  if (mHFile)
  {
    File::Close(mHFile);
    mHFile = nullptr;
  }
}

}
