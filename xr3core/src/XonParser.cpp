//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/XonParser.hpp"
#include "xr/ParserCore.hpp"
#include "xr/debug.hpp"
#include <string>

namespace xr
{

//==============================================================================
bool XonParser::Parse(char const* string, size_t length, Callback callback, void* userData)
{
  XR_ASSERT(XonParser, string != nullptr);
  m_state = { 0, 0, 0, string };

  ParserCore core;
  core.SetBuffer(string, length);

  m_callback = callback;
  m_callbackUser = userData;

  // Look for an object. Drop whitespace and comments, find {, skip it.
  // After parsing the root object and processing all whitespace and comments,
  // we should be at the end of the buffer.
  const char* charp = ProcessComments(core);
  bool success = !core.IsOver(charp)
    && *charp == kObjectBegin
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
  const char* charp = nullptr;
  if (result)
  {
    ++m_state.depth;

    charp = ProcessComments(pc);
    result = !pc.IsOver(charp);
  }

  if (result)
  {
    if (*charp == kObjectEnd) // empty object
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

      charp = ProcessComments(pc);
      result = !pc.IsOver(charp);
      if (!result)
      {
        break;
      }

      // A colon is only a legal next character if we've parsed a string;
      // this will now be a key.
      if (*charp == kKey)
      {
        // Unquoted empty / null string is invalid.
        result = er.type == ElementResult::Type::String
          && (str.isQuoted || str.length > 0)
          && DoCallback(Event::Key, &str);
        if (result)
        {
          charp = pc.SkipChar();

          result = !pc.IsOver(charp);
          if (!result)
          {
            break;
          }

          charp = ProcessComments(pc);
          result = !pc.IsOver(charp);
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

      charp = ProcessComments(pc);
      result = !pc.IsOver(charp);
      if (!result)
      {
        break;
      }

      // Require comma or object end.
      const bool isObjectEnd = *charp == kObjectEnd;
      result = (*charp == kNextValue || isObjectEnd);
      if (result)
      {
        // Length == -1 signifies an unquoted empty string, which is not invalid
        // here, but we'll ignore it, EXCEPT if it's a null string.
        if (er.type == ElementResult::Type::String
          && (str.length > 0 || str.isQuoted || str.start == nullptr))
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
  const char* start = pc.GetChar();
  XR_ASSERT(XonParser, !pc.IsOver(start));
  ElementResult er = { true, *start == kObjectBegin ?
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
  const char* start = pc.GetChar();

  XR_ASSERT(XonParser, !pc.IsOver(start));
  XR_ASSERT(XonParser, !isspace(*start));
  bool result = !(*start == kKey || *start == kNextValue);
  if (result)
  {
    const bool isQuoted = *start == kQuot;
    strOut.isQuoted = isQuoted;
    if (isQuoted)
    {
      // Find the first unescaped quote.
      start = pc.SkipChar();
      result = !pc.IsOver(start);
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
        strOut.length = pc.GetChar() - start;
        strOut.start = start;
        pc.SkipChar();
      }
    }
    else
    {
      // No quotes; find the end of the string: special character, space or end of
      // buffer. Even though the buffer really should not end at this point, this is
      // not an error from the viewpoint of the string, so we're not flagging it here.
      const char* end = pc.GetChar(); // same as start
      while (!isspace(*end) &&
        !(*end == kNextValue || *end == kKey || *end == kComment || *end == kObjectEnd ||
          *end == kObjectBegin))
      {
        end = pc.SkipChar();
        if (pc.IsOver(end))
        {
          break;
        }
      }

      strOut.length = end - start;
      strOut.start = start;
    }
  }

  return result;
}

//==============================================================================
const char* XonParser::ProcessComments(ParserCore& pc)
{
  const char* charp(pc.ExpectChar());
  while (!pc.IsOver(charp) && *charp == kComment)
  {
    // Skip all characters in this row.
    int row = pc.GetRow();
    while (!pc.IsOver(pc.SkipChar()) && pc.GetRow() == row)
    {}

    charp = pc.ExpectChar();
  }
  return charp;
}

//==============================================================================
bool  XonParser::DoCallback(Event e, const String* data)
{
  XR_ASSERT(XonParser, m_callback != nullptr);
  return (*m_callback)(e, data, m_callbackUser);
}

} // xr
