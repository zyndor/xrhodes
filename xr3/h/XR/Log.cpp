//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "Log.hpp"
#include "File.hpp"
#include <XR/debug.hpp>
#include <cstring>

namespace XR
{

//==============================================================================
static int32_t  s_hLogFile(File::INVALID_HANDLE);

//==============================================================================
void Log::Init()
{
  XR_ASSERTMSG(Log, s_hLogFile == File::INVALID_HANDLE, ("Already initialised!"));
  s_hLogFile = File::Open("log.txt", "wb");
}

//==============================================================================
void Log::Exit()
{
  XR_ASSERTMSG(Log, s_hLogFile != File::INVALID_HANDLE, ("Not initialised."));
  File::Close(s_hLogFile);
  s_hLogFile = File::INVALID_HANDLE;
}

//==============================================================================
void  Log::Write(const char* pString )
{
#if defined XR_DEBUG
  bool  logSuccess(File::Write(pString, strlen(pString), 1, s_hLogFile) != 0 &&
    File::Write("\n", 1, 1, s_hLogFile) != 0);
  XR_ASSERT(Log, logSuccess);
#else
  File::Write(pString, strlen(pString), 1, s_hLogFile);
  File::Write("\n", 1, 1, s_hLogFile);
#endif  // XR_DEBUG
}


} // XR

