//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/XonParser.hpp"
#include "XR/ParserCore.hpp"
#include "XR/debug.hpp"
#include <string>

namespace XR
{

//==============================================================================
bool XonParser::Parse(char const* pBuffer, size_t length, Callback pCallback, void* pUser)
{
  XR_ASSERT(XonParser, pBuffer != nullptr);
  m_state = { 0, 0, 0, pBuffer };

  ParserCore core;
  core.SetBuffer(pBuffer, length);

  m_pCallback = pCallback;
  m_pCallbackUser = pUser;

  // Look for an object. Drop whitespace and comments, find {, skip it.
  // After parsing the root object and processing all whitespace and comments,
  // we should be at the end of the buffer.
  const char* pChar = ProcessComments(core);
  bool success = !core.IsOver(pChar)
    && *pChar == kObjectBegin
    && !core.IsOver(core.SkipChar())
    && ParseObject(core)
    && core.IsOver(ProcessComments(core));

  m_state = { core.GetRow(), core.GetColumn(), m_state.depth, core.GetChar() };
  return success;
}

//==============================================================================
bool XonParser::ParseObject(ParserCore& pc)
{
  bool result = DoCallback(Event::ObjectBegin, nullptr);
  const char* pChar = nullptr;
  if (result)
  {
    ++m_state.depth;

    pChar = ProcessComments(pc);
    result = !pc.IsOver(pChar);
  }

  if (result)
  {
    if (*pChar == kObjectEnd) // empty object
    {
      pc.SkipChar();
      --m_state.depth;

      result = m_state.depth >= 0
        && DoCallback(Event::ObjectEnd, nullptr);
    }
    else while (result) // some value or key.
    {
      // Parse object (and children) or string.
      String str = { nullptr, 0 };
      auto er = ParseElement(pc, str);
      if (!er.isValid)
      {
        result = er.isValid;
        break;
      }

      pChar = ProcessComments(pc);
      result = !pc.IsOver(pChar);
      if (!result)
      {
        break;
      }

      // A colon is only a legal next character if we've parsed a string;
      // this will now be a key.
      if (*pChar == kKey)
      {
        // Unquoted empty / null string is invalid.
        result = er.type == ElementResult::Type::String
          && (str.isQuoted || str.length > 0)
          && DoCallback(Event::Key, &str);
        if (result)
        {
          pChar = pc.SkipChar();

          result = !pc.IsOver(pChar);
          if (!result)
          {
            break;
          }

          pChar = ProcessComments(pc);
          result = !pc.IsOver(pChar);
          if (!result)
          {
            break;
          }

          er = ParseElement(pc, str);
          if (!er.isValid)
          {
            result = er.isValid;
            break;
          }
        }
        else
        {
          break;
        }
      }

      pChar = ProcessComments(pc);
      result = !pc.IsOver(pChar);
      if (!result)
      {
        break;
      }

      // Require comma or object end.
      const bool isObjectEnd = *pChar == kObjectEnd;
      result = (*pChar == kNextValue || isObjectEnd);
      if (result)
      {
        // Length == -1 signifies an unquoted empty string, which is not invalid
        // here, but we'll ignore it, EXCEPT if it's a null string.
        if (er.type == ElementResult::Type::String
          && (str.length > 0 || str.isQuoted || str.pStart == nullptr))
        {
          result = DoCallback(Event::Value, &str);
        }

        if (result)
        {
          pc.SkipChar();
          ProcessComments(pc);

          if (isObjectEnd)
          {
            --m_state.depth;

            result = m_state.depth >= 0
              && DoCallback(Event::ObjectEnd, nullptr);
            break;
          }
        }
      }
    }
  }
  return result;
}

//==============================================================================
XonParser::ElementResult XonParser::ParseElement(ParserCore& pc, String& strOut)
{
  const char* pStart = pc.GetChar();
  XR_ASSERT(XonParser, !pc.IsOver(pStart));
  ElementResult er = { true, *pStart == kObjectBegin ?
    ElementResult::Type::Object : ElementResult::Type::String };
  switch(er.type)
  {
  case ElementResult::Type::Object:
    er.isValid = !pc.IsOver(pc.SkipChar()) && ParseObject(pc);
    break;

  case ElementResult::Type::String:
    er.isValid = ParseString(pc, strOut);
    break;
  }
  return er;
}

//==============================================================================
bool XonParser::ParseString(ParserCore& pc, String& strOut)
{
  const char* pStart = pc.GetChar();

  XR_ASSERT(XonParser, !pc.IsOver(pStart));
  XR_ASSERT(XonParser, !isspace(*pStart));
  bool result = !(*pStart == kKey || *pStart == kNextValue);
  if (result)
  {
    const bool isQuoted = *pStart == kQuot;
    strOut.isQuoted = isQuoted;
    if (isQuoted)
    {
      // Find the first unescaped quote.
      pStart = pc.SkipChar();
      result = !pc.IsOver(pStart);
      while(result)
      {
        result = !pc.IsOver(pc.RequireChar(kQuot));
        if (result)
        {
          XR_ASSERT(XonParser, pc.GetRow() > 1 || pc.GetColumn() > 1);  // prevent checking before the very first char. This is not a realistic scenario: the first thing in the buffer can not be a quoted string (which is the only place it matters).
          if (*(pc.GetChar() - 1) == kEscape)
          {
            // Skip it and keep looking till the bitter end.
            result = !pc.IsOver(pc.SkipChar());
          }
          else
          {
            break;
          }
        }
      }

      if (result)
      {
        strOut.length = pc.GetChar() - pStart;
        strOut.pStart = pStart;
        pc.SkipChar();
      }
    }
    else
    {
      // No quotes; find the end of the string: special character, space or end of
      // buffer. Even though the buffer really should not end at this point, this is
      // not an error from the viewpoint of the string, so we're not flagging it here.
      const char* pEnd = pc.GetChar(); // same as start
      while (!isspace(*pEnd) &&
        !(*pEnd == kNextValue || *pEnd == kKey || *pEnd == kComment || *pEnd == kObjectEnd ||
          *pEnd == kObjectBegin))
      {
        pEnd = pc.SkipChar();
        if (pc.IsOver(pEnd))
        {
          break;
        }
      }

      strOut.length = pEnd - pStart;
      strOut.pStart = pStart;
    }
  }

  return result;
}

//==============================================================================
const char* XonParser::ProcessComments(ParserCore& pc)
{
  const char* pChar(pc.ExpectChar());
  while (!pc.IsOver(pChar) && *pChar == kComment)
  {
    // Skip all characters in this row.
    int row = pc.GetRow();
    while (!pc.IsOver(pc.SkipChar()) && pc.GetRow() == row)
    {}

    pChar = pc.ExpectChar();
  }
  return pChar;
}

//==============================================================================
bool  XonParser::DoCallback(Event e, const String* pData)
{
  XR_ASSERT(XonParser, m_pCallback != nullptr);
  return (*m_pCallback)(e, pData, m_pCallbackUser);
}

} // XR
