//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/Log.hpp"
#include "XR/File.hpp"
#include "XR/debug.hpp"
#include <cstring>

namespace XR
{

//==============================================================================
static File::Handle  s_hLogFile = nullptr;

//==============================================================================
void Log::Init()
{
  XR_ASSERTMSG(Log, !s_hLogFile, ("Already initialised!"));
  s_hLogFile = File::Open("log.txt", "wb");
}

//==============================================================================
void Log::Exit()
{
  XR_ASSERTMSG(Log, s_hLogFile, ("Not initialised."));
  File::Close(s_hLogFile);
  s_hLogFile = nullptr;
}

//==============================================================================
void  Log::Write(const char* pString )
{
#if defined XR_DEBUG
  size_t size = strlen(pString);
  bool  logSuccess(File::Write(pString, 1, size, s_hLogFile) != size &&
    File::Write("\n", 1, 1, s_hLogFile) != 1);
  XR_ASSERT(Log, logSuccess);
#else
  File::Write(pString, 1, strlen(pString), s_hLogFile);
  File::Write("\n", 1, 1, s_hLogFile);
#endif  // XR_DEBUG
}


} // XR

