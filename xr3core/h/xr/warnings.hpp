#ifndef XR_WARNINGS_HPP
#define XR_WARNINGS_HPP
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
#include "xr/stringify.hpp"

#if defined XR_COMPILER_MSVC

///@brief Shorthand for an warning related pragma in MSVC; compiles out when using
/// other toolchains.
#define XR_MSVC_WARNING(x) __pragma(warning(x))
#define XR_GCC_WARNING(x)
#define XR_CLANG_WARNING(x)

///@brief Pushes a diagnostics control frame.
#define XR_WARNINGS_PUSH XR_MSVC_WARNING(push)

///@brief Pushes a diagnostics control frame.
#define XR_WARNINGS_POP XR_MSVC_WARNING(pop)

#else

#define XR_DETAIL_PRAGMA(x) _Pragma(XR_STRINGIFY(x))

#define XR_MSVC_WARNING(x)

#if defined XR_COMPILER_GCC

///@brief Shorthand for an warning related pragma in GCC; compiles out when using
/// other toolchains.
#define XR_GCC_WARNING(x) XR_DETAIL_PRAGMA(GCC diagnostic x)
#define XR_CLANG_WARNING(x)

#define XR_WARNINGS_PUSH XR_GCC_WARNING(push)
#define XR_WARNINGS_POP XR_GCC_WARNING(pop)

#elif defined XR_COMPILER_CLANG

#define XR_GCC_WARNING(x)
///@brief Shorthand for an warning related pragma in Clang; compiles out when using
/// other toolsets.
#define XR_CLANG_WARNING(x) XR_DETAIL_PRAGMA(clang diagnostic x)

#define XR_WARNINGS_PUSH XR_CLANG_WARNING(push)
#define XR_WARNINGS_POP XR_CLANG_WARNING(pop)

#endif

#endif

///@brief Convenience macro for instructing the compiler to ignore deprecated declarations
/// on all supported toolchains.
#define XR_WARNINGS_IGNORE_DEPRECATION \
  XR_MSVC_WARNING(disable: 4996) \
  XR_CLANG_WARNING(ignored "-Wdeprecated-declarations") \
  XR_GCC_WARNING(ignored "-Wdeprecated-declarations")

#endif //XR_WARNINGS_HPP
