//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xm.hpp"

int main(int argc, char** argv)
{
  if (argc > 1)
  {
    xm::SetFilter(argv[1]);
  }
  return xm::RunTests();
}
