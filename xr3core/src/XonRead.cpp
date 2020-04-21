//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/xon/XonRead.hpp"

namespace xr
{

//==============================================================================
char const* XonRead::CString(XonEntity const & xon)
{
  return xon.ToValue().GetString();
}

//==============================================================================
std::string XonRead::String(XonEntity const & xon)
{
  return std::string(CString(xon));
}

}
