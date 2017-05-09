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
void  ParserCore::SetBuffer(const char* parBuffer, int size)
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
bool  ParserCore::IsNewLine() const
{
  XR_ASSERT(ParserCore, m_p0 != 0);
  return *m_p0 == '\n' || *m_p0 == '\r';
}

//==============================================================================
const char* ParserCore::ExpectChar()
{
  XR_ASSERT(ParserCore, m_p0 != 0);
  int col(m_column);
  int row(m_row);
  
  while (m_p0 != m_p1 && iswspace(*m_p0))
  {
    if (IsNewLine())
    {
      col = 1;
      ++row;
    }
    else
    {
      ++col;
    }
    ++m_p0;
  }
  m_row = row;
  m_column = col;
  return m_p0;
}

//==============================================================================  
const char* ParserCore::RequireChar(int c)
{
  XR_ASSERT(ParserCore, m_p0 != 0);
  int col(m_column);
  int row(m_row);
  
  while (m_p0 != m_p1 && *m_p0 != c)
  {
    if (IsNewLine())
    {
      col = 1;
      ++row;
    }
    else
    {
      ++col;
    }
    ++m_p0;
  }
  m_row = row;
  m_column = col;
  return m_p0;
}

//==============================================================================
const char* ParserCore::SkipChar()
{
  XR_ASSERT(ParserCore, m_p0 != 0);
  if (m_p0 != m_p1)
  {
    if (IsNewLine())
    {
      m_column = 1;
      ++m_row;
    }
    else
    {
      ++m_column;
    }
    ++m_p0;
  }
  return m_p0;
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
