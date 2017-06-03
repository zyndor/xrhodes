//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <XR/stringutils.hpp>
#include <algorithm>
#include <cstring>

namespace XR
{

//==============================================================================
char const* Replace(char const* original, size_t originalSize, char const* find,
  char const* replace, size_t bufferSize, char* buffer, size_t& replacedSize)
{
  const auto findLen = strlen(find);
  const auto replaceLen = strlen(replace);
  auto write = buffer;
  auto originalEnd = original + originalSize;
  auto writeEnd = buffer + bufferSize;
  while (original != originalEnd && write != writeEnd)
  {
    // TODO: instead of repeatedly calling strncmp, do an internal loop to find long
    // partial matches and copy them whole?
    if (findLen <= (originalEnd - original) && strncmp(original, find, findLen) == 0)
    {
      auto copyLen = std::min(size_t(writeEnd - write), replaceLen);
      strncpy(write, replace, copyLen);
      write += copyLen;
      original += findLen;
    }
    else
    {
      *write = *original;
      ++write;
      ++original;
    }
  }

  replacedSize = write - buffer;
  return buffer;
}

//==============================================================================
char const* Replace(char const* original, char const* find, char const* replace,
  size_t bufferSize, char* buffer, size_t& replacedSize)
{
  return Replace(original, strlen(original), find, replace, bufferSize, buffer,
    replacedSize);
}

} // XR
