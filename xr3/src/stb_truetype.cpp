//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/warnings.hpp"

XR_CLANG_WARNING(ignored "-Wdouble-promotion")
XR_CLANG_WARNING(ignored "-Wmissing-declarations")
XR_CLANG_WARNING(ignored "-Wold-style-cast")

XR_GCC_WARNING(ignored "-Wdouble-promotion")
XR_GCC_WARNING(ignored "-Wmissing-declarations")
XR_GCC_WARNING(ignored "-Wold-style-cast")

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
