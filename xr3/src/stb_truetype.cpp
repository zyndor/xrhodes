//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/platform.hpp"

#if defined XR_COMPILER_GCC
#pragma GCC diagnostic ignored "-Wdouble-promotion"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
