#pragma once

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
    File::Exit();
  }
};

}
