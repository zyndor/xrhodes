#ifndef XRUT_FILELIFECYCLEMANAGER_HPP
#define XRUT_FILELIFECYCLEMANAGER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/File.hpp"

namespace xr
{

// Test fixtures may have an instance of this guy to ensure that
// File is initialised and uninitialised with as little extra code
// as possible.
class FileLifeCycleManager
{
public:
  FileLifeCycleManager()
  {
    File::Init({ ".", DATA_PATH });
  }

  ~FileLifeCycleManager()
  {
    File::Shutdown();
  }
};

}

#endif //XRUT_FILELIFECYCLEMANAGER_HPP
