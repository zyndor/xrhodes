//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/stringutils.hpp"
#include "XR/debug.hpp"
#include <algorithm>
#include <cctype>
#include <cstring>

namespace XR
{

//==============================================================================
char const* Replace(char const* original, size_t originalSize, char const* find,
  char const* replace, size_t bufferSize, char* buffer, size_t& processedSize)
{
  const auto findLen = strlen(find);
  const auto replaceLen = strlen(replace);
  auto originalEnd = original + originalSize;
  auto write = buffer;
  auto writeEnd = buffer + bufferSize;
  while (original != originalEnd && write != writeEnd)
  {
    // TODO: instead of repeatedly calling strncmp, do an internal loop to find
    // long partial matches and copy them whole?
    if (findLen <= static_cast<size_t>(originalEnd - original)
      && strncmp(original, find, findLen) == 0)
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

  if (write != writeEnd)
  {
    *write = '\0';
  }
  processedSize = write - buffer;
  return buffer;
}

//==============================================================================
char const* Replace(char const* original, char const* find, char const* replace,
  size_t bufferSize, char* buffer, size_t& processedSize)
{
  return Replace(original, strlen(original), find, replace, bufferSize, buffer,
    processedSize);
}

//==============================================================================
const char kHexDigits[] = {
  '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

char* Char2Hex(char c, char buffer[2])
{
  const int d0 = (c & 0xf0) >> 4;
  const int d1 = c & 0xf;
  *buffer = kHexDigits[d0];
  ++buffer;
  *buffer = kHexDigits[d1];
  ++buffer;
  return buffer;
}

//==============================================================================
char const* UrlEncode(char const* original, size_t originalSize, size_t bufferSize,
  char* buffer, size_t& processedSize)
{
  XR_ASSERT(UrlEncode, original != nullptr);
  auto originalEnd = original + originalSize;
  auto write = buffer;
  auto writeEnd = buffer + bufferSize;
  while (original != originalEnd && write != writeEnd)
  {
    const char c = *original;
    if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
    {
      *write = c;
      ++write;
    }
    else if(write + 3 <= writeEnd)
    {
      *write = '%';
      ++write;
      write = Char2Hex(c, write);
    }
    else
    {
      break;
    }
    ++original;
  }

  if (write != writeEnd)
  {
    *write = '\0';
  }
  processedSize = write - buffer;
  return buffer;
}

//==============================================================================
char const* UrlEncode(char const* original, size_t bufferSize, char* buffer,
  size_t& processedSize)
{
  return UrlEncode(original, strlen(original), bufferSize, buffer, processedSize);
}

} // XR
