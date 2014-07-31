//
// Nuclear Heart Games
// XRhodes
//
// ParserCore.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    24/05/2014
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_PARSERCORE_HPP
#define XR_PARSERCORE_HPP

#include "types.hpp"

namespace XR
{

//==============================================================================
///@brief Core functionality for parsing text formats, with row and column
/// tracking.
class ParserCore
{
public:
  // structors
  ParserCore();
  ~ParserCore();
  
  // general
  ///@brief Resets the internal state and sets up to read @a size characters
  /// from @a parBuffer. If @a size is 0, it will set it to strlen(parBuffer).
  void        SetBuffer(const char* parBuffer, int size);
  
  int         GetRow() const;
  int         GetColumn() const;
  
  ///@return  Pointer to the character we're currently on.
  const char* GetChar() const;
  
  ///@brief   Moves to the next non-whitespace character, if any.
  ///@return  Pointer to the character we've ended up at.
  const char* ExpectChar();
  
  ///@brief   Moves to the next character that matches @a c, if any.
  ///@return  Pointer to the character we've ended up at.
  const char* RequireChar(int c);
  
  ///@brief   Moves to the next character, if any.
  ///@return  Pointer to the character we've ended up at.
  const char* SkipChar();
  
  ///@brief   SkipChar()s @a numChars times, but not past the end.
  ///@return  Pointer to the character we've ended up at.
  const char* FastForward(int numChars);
  
  ///@return  Number of characters left.
  int         GetNumCharsLeft() const;
  
  ///@return  Whether @a p is at or past the end of the buffer.
  bool        IsOver(const char* p) const;

protected:
  // data
  const char*   m_p0; // current character to process; no ownership
  const char*   m_p1; // end of buffer, not processed; no ownership
  int           m_row; // 1-based, human readable
  int           m_column; // 1-based, human readable
  
  // internal
  bool  IsNewLine() const;
};

//==============================================================================
// implementation
//==============================================================================
inline
int ParserCore::GetRow() const
{
  return m_row;
}

//==============================================================================
inline
int ParserCore::GetColumn() const
{
  return m_column;
}

//==============================================================================
inline
const char* ParserCore::GetChar() const
{
  return m_p0;
}

//==============================================================================
inline
int ParserCore::GetNumCharsLeft() const
{
  return m_p1 - m_p0;
}

//==============================================================================
inline
bool  ParserCore::IsOver(const char* p) const
{
  return p >= m_p1;
}

} // XR

#endif // XR_PARSERCORE_HPP
