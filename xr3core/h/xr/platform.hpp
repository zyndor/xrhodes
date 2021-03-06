#ifndef XR_PLATFORM_HPP
#define XR_PLATFORM_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================

// compilers
#if defined(_MSC_VER)
#define XR_COMPILER_MSVC
#elif defined(__clang__)
#define XR_COMPILER_CLANG
#elif defined(__GNUC__)
#define XR_COMPILER_GCC
#else
#error "Unsupported compiler!"
#endif  // compilers

// cpus
#if\
  defined(__i386__)||\
  defined(_M_IX86)||\
  defined(_M_X64)||\
  defined(__x86_64__)||\
  defined(__amd64__)
#define XR_CPU_INTEL
#elif\
  defined(_M_ARM)||\
  defined(__arm__)||\
  defined(__aarch64__)
#define XR_CPU_ARM
#else
#error "Unsupported cpu!"
#endif // cpus

// architectures
#if\
  defined(_M_X64)||\
  defined(__x86_64__)||\
  defined(__amd64__)||\
  defined(__aarch64__)
#define XR_ARCH_64
#elif\
  defined(_M_IX86)||\
  defined(_M_ARM)||\
  defined(__i386__)||\
  defined(__arm__)
#define XR_ARCH_32
#else
#error "Unsupported architecture!"
#endif // architectures

// platforms
// Windows
#if\
  defined(_WIN32)||\
  defined(_WIN64)
#define XR_PLATFORM_WINDOWS
// OSX
#elif\
  defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
#define XR_PLATFORM_OSX
// iOS
#elif\
  defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__)||\
  defined(__ENVIRONMENT_TV_OS_VERSION_MIN_REQUIRED__)
#define XR_PLATFORM_IOS
// Android
#elif defined(__ANDROID__)
#define XR_PLATFORM_ANDROID
// Emscripten
#elif defined(__EMSCRIPTEN__)
#define XR_PLATFORM_EMSCRIPTEN
// Linux - check this after the platforms that may define it
#elif\
  defined(__linux__)||\
  defined(__riscv__)
#define XR_PLATFORM_LINUX
#else
#error "Unsupported platform!"
#endif // platforms

#endif //XR_PLATFORM_HPP
