//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <cstring>
#include <string>
#include <XR/ParserCore.hpp>
#include <XR/debug.hpp>

namespace XR
{

//==============================================================================
ParserCore::ParserCore()
: m_p0(0),
  m_p1(0),
  m_row(0),
  m_column(0)
{}

//==============================================================================
ParserCore::~ParserCore()
{}

//==============================================================================
void  ParserCore::SetBuffer(const char* parBuffer, size_t size)
{
  XR_ASSERT(ParserCore, parBuffer != 0);
  XR_ASSERT(ParserCore, size >= 0);
  if (size == 0)
  {
    size = strlen(parBuffer);
  }
  
  m_p0 = parBuffer;
  m_p1 = parBuffer + size;
  m_row = 1;
  m_column = 1;
}

//==============================================================================
const char* ParserCore::ExpectChar()
{
  XR_ASSERT(ParserCore, m_p0 != 0);
  while (m_p0 != m_p1 && isspace(*m_p0))
  {
    SkipChar();
  }
  return m_p0;
}

//==============================================================================  
const char* ParserCore::RequireChar(int c)
{
  XR_ASSERT(ParserCore, m_p0 != 0);
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
      auto pNext = p0 + 1;
      if (pNext != m_p1 && *pNext == '\n')
      {
        p0 = pNext;
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
  const char* pResult(m_p1);
  while(numChars > 0 && m_p0 != m_p1)
  {
    pResult = SkipChar();
    --numChars;
  }
  return pResult;
}

} // XR