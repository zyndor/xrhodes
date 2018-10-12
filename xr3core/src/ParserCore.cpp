//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include <cstring>
#include <string>
#include "xr/strings/ParserCore.hpp"
#include "xr/debug.hpp"

namespace xr
{

//==============================================================================
ParserCore::ParserCore()
: m_p0(nullptr),
  m_p1(nullptr),
  m_row(0),
  m_column(0)
{}

//==============================================================================
ParserCore::~ParserCore()
{}

//==============================================================================
void  ParserCore::SetBuffer(const char* buffer, size_t size)
{
  XR_ASSERT(ParserCore, buffer != nullptr);
  XR_ASSERT(ParserCore, size >= 0);
  if (size == 0)
  {
    size = strlen(buffer);
  }

  m_p0 = buffer;
  m_p1 = buffer + size;
  m_row = 1;
  m_column = 1;
}

//==============================================================================
const char* ParserCore::ExpectChar()
{
  XR_ASSERT(ParserCore, m_p0 != nullptr);
  while (m_p0 != m_p1 && isspace(*m_p0))
  {
    SkipChar();
  }
  return m_p0;
}

//==============================================================================
const char* ParserCore::RequireChar(int c)
{
  XR_ASSERT(ParserCore, m_p0 != nullptr);
  while (m_p0 != m_p1 && *m_p0 != c)
  {
    SkipChar();
  }
  return m_p0;
}

//==============================================================================
const char* ParserCore::SkipChar()
{
  XR_ASSERT(ParserCore, m_p0 != nullptr);
  const char* p0 = m_p0;
  if (p0 != m_p1)
  {
    // check for new line. CR or LF
    const bool isCR = *p0 == '\r';
    const bool isNewLine = isCR || *p0 == '\n';
    if (isCR) // CR might be CRLF in the making, which we'll handle as one line break.
    {
      auto next = p0 + 1;
      if (next != m_p1 && *next == '\n')
      {
        p0 = next;
      }
    }
    ++p0;

    // update the cursor
    if (isNewLine)
    {
      m_column = 1;
      ++m_row;
    }
    else
    {
      ++m_column;
    }

    // update m_p0
    m_p0 = p0;
  }
  return p0;
}

//==============================================================================
const char* ParserCore::FastForward(int numChars)
{
  XR_ASSERT(ParserCore, numChars >= 0);
  const char* result = m_p1;
  while(numChars > 0 && m_p0 != m_p1)
  {
    result = SkipChar();
    --numChars;
  }
  return result;
}

} // xr
