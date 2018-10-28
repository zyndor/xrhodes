//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/JsonParser.hpp"
#include "xr/debug.hpp"
#include <algorithm>
#include <limits>
#include <ctype.h>
#include <cstring>
#include <cmath>
#include <cstdio>
#include <cstdlib>

namespace xr
{
namespace JSON
{

//==============================================================================
Parser::Parser(int maxDepth)
: m_state(),
  m_depth(0),
  m_maxDepth(maxDepth),
  m_handler(nullptr),
  m_onAtMaxDepth(nullptr)
{
  XR_ASSERTMSG(Json::Parser, maxDepth > 1, ("%d is not a sensible value for maxDepth.",
    maxDepth));
}

//==============================================================================
Parser::~Parser ()
{}

//==============================================================================
bool  Parser::Parse(const char* string, size_t size, const EntityCallback& handler)
{
  XR_ASSERT(Json::Parser, string != nullptr);
  XR_ASSERT(Json::Parser, size >= 0);
  m_state.SetBuffer(string, size);
  m_depth = 0;
  m_handler.reset(static_cast<EntityCallback*>(handler.Clone()));

  const char* charp(m_state.ExpectChar()); // drop leading whitespace
  bool        result(!m_state.IsOver(charp));
  if (result)
  {
    bool  isObject(*charp == kObjectBegin);
    bool  isArray(*charp == kArrayBegin);

    result = (isObject || isArray) && !m_state.IsOver(m_state.SkipChar());
    if (result) // check exceeding max depth (not an error)
    {
      result = isObject ? ParseObject() : ParseArray();
    }
  }

  return result;
}

//==============================================================================
bool  Parser::ParseArray()
{
  OnEntity(E_ARRAY_BEGIN, nullptr);
  IncreaseDepth();
  const char* charp = m_state.ExpectChar(); // look for array end or value
  bool result = !m_state.IsOver(charp);
  if (result)
  {
    if (*charp == kArrayEnd)
    {
      DecreaseDepth();
      m_state.SkipChar();
    }
    else while (result)
    {
      result = ParseValue();
      if (!result)
      {
        break;
      }

      charp = m_state.ExpectChar();
      result = !m_state.IsOver(charp) &&
        (*charp == kComma || *charp == kArrayEnd);
      if (!result)
      {
        break;
      }

      m_state.SkipChar(); // leave character
      if (*charp == kArrayEnd)
      {
        DecreaseDepth();
        break;
      }
    }
  }
  OnEntity(E_ARRAY_END, nullptr);
  return result;
}

//==============================================================================
bool  Parser::ParseObject()
{
  OnEntity(E_OBJECT_BEGIN, nullptr);
  IncreaseDepth();
  const char* charp(m_state.ExpectChar());
  bool  result(!m_state.IsOver(charp));
  if (result)
  {
    if (*charp == kObjectEnd)
    {
      DecreaseDepth();
      m_state.SkipChar();
    }
    else while (result)
    {
      charp = m_state.ExpectChar();
      result = *charp == kQuot;
      if (!result)
      {
        break;
      }

      const char* key(m_state.SkipChar()); // leave the quot
      result = !m_state.IsOver(key);
      if (!result)
      {
        break;
      }

      const char* keyEnd(m_state.RequireChar(kQuot)); // get other quot
      result = !m_state.IsOver(keyEnd);
      if (!result)
      {
        break;
      }

      XR_ASSERT(Json::Parser, keyEnd >= key);
      String  str = { key, size_t(keyEnd - key) };
      OnEntity(E_KEY, &str);

      result = !m_state.IsOver(m_state.SkipChar());
      if (!result)
      {
        break;
      }

      charp = m_state.ExpectChar();  // look for colon
      result = !m_state.IsOver(charp) && *charp == kColon;
      if (!result)
      {
        break;
      }

      result = !m_state.IsOver(m_state.SkipChar());
      if (!result)
      {
        break;
      }

      result = ParseValue();
      if (!result)
      {
        break;
      }

      charp = m_state.ExpectChar();
      result = !m_state.IsOver(charp) &&
        (*charp == kComma || *charp == kObjectEnd);
      if (!result)
      {
        break;
      }

      m_state.SkipChar();
      if (*charp == kObjectEnd)
      {
        DecreaseDepth();
        break;
      }
    }
  }
  OnEntity(E_OBJECT_END, nullptr);
  return result;
}

//==============================================================================
bool  Parser::ParseValue()
{
  const char* charp = m_state.ExpectChar();
  bool  result(!m_state.IsOver(charp));
  if (result)
  {
    const size_t kBufferSize = 64; // bytes enough for everyone? (for conversion of numericals.)
    char  buffer[kBufferSize];

    if (*charp == kQuot) // string
    {
      charp = m_state.SkipChar();
      result = !m_state.IsOver(charp);
      if (result)
      {
        const char* pValueEnd(m_state.RequireChar(kQuot)); // look for quot pair
        result = !m_state.IsOver(pValueEnd);
        if (result)
        {
          String  str = { charp, size_t(pValueEnd - charp) };
          OnEntity(E_VALUE, &str);
          m_state.SkipChar();
        }
      }
    }
    else if (isdigit(*charp) || *charp == '-')
    {
      const char* valueEnd(m_state.SkipChar());
      const char* pExponential = nullptr;
      const char* pDecimal = nullptr;
      while (result && !m_state.IsOver(valueEnd) &&
        !(*valueEnd == kComma || *valueEnd == kObjectEnd || *valueEnd == kArrayEnd ||
          isspace(*valueEnd)))
      {
        const bool  isExponential(*valueEnd == kExponential);
        const bool  isDecimal(*valueEnd == kDecimal);
        result = isdigit(*valueEnd) ||
          // first decimal point
          (*valueEnd == kDecimal && pDecimal == nullptr) ||
          // first exponential, not straight after a minus sign
          (isExponential && pExponential == nullptr && *(valueEnd - 1) != '-') ||
          // minus sign following an exponential
          (*valueEnd == '-' && pExponential == valueEnd - 1);
        if(result)
        {
          if(isDecimal)
          {
            pDecimal = valueEnd;
          }
          else if(isExponential)
          {
            pExponential = valueEnd;
          }

          valueEnd = m_state.SkipChar();
          result = !m_state.IsOver(valueEnd);
        }
      }

      if (result)
      {
        result = !m_state.IsOver(valueEnd);
      }

      if (result)
      {
        size_t len = valueEnd - charp;

        double  value = atof(charp);
        double  absValue = std::abs(value);
        if(absValue - std::floor(absValue) < std::numeric_limits<double>::epsilon())
        {
          len = std::snprintf(buffer, kBufferSize - 1, "%d", int(value));
        }
        else
        {
          len = std::min(kBufferSize - 1, len);
          std::strncpy(buffer, charp, len);
          buffer[len] = '\0';
        }

        String  str = { buffer, static_cast<size_t>(len) };
        OnEntity(E_VALUE, &str);
      }
    }
    else if (isalpha(*charp))
    {
      const char* pValueEnd = m_state.SkipChar();
      while (!m_state.IsOver(pValueEnd) &&
        !(*pValueEnd == kComma || *pValueEnd == kObjectEnd || *pValueEnd == kArrayEnd ||
          isspace(*pValueEnd)))
      {
        pValueEnd = m_state.SkipChar();
      }

      const size_t len = pValueEnd - charp;
      if (strncmp(charp, kFalse, len) == 0)
      {
        auto size = size_t(std::snprintf(buffer, kBufferSize - 1, "0"));
        String  str = { buffer, size };
        OnEntity(E_VALUE, &str);
      }
      else if (strncmp(charp, kTrue, len) == 0)
      {
        auto size = size_t(std::snprintf(buffer, kBufferSize - 1, "1"));
        String  str = { buffer, size };
        OnEntity(E_VALUE, &str);
      }
      else if (strncmp(charp, kNull, len) == 0)
      {
        String  str = { nullptr, 0 };
        OnEntity(E_VALUE, &str);
      }
      else
      {
        result = false;
      }
    }
    else if (*charp == kObjectBegin)
    {
      if (!m_state.IsOver(m_state.SkipChar()))
      {
        result = ParseObject();
      }
    }
    else if (*charp == kArrayBegin)
    {
      if (!m_state.IsOver(m_state.SkipChar()))
      {
        result = ParseArray();
      }
    }
    else
    {
      result = false;
    }
  }
  return result;
}

//==============================================================================
void  Parser::OnEntity(Event e, const String* data)
{
  if (m_handler && m_depth <= m_maxDepth)
  {
    m_handler->Call(e, data);
  }
}

//==============================================================================
void Parser::IncreaseDepth()
{
  ++m_depth;
  if (m_depth == m_maxDepth + 1)
  {
    DoAtMaxDepth(true);
  }
}

//==============================================================================
void Parser::DecreaseDepth()
{
  --m_depth;
  if (m_depth == m_maxDepth)
  {
    DoAtMaxDepth(false);
  }
}

//==============================================================================
void Parser::DoAtMaxDepth(bool entered)
{
  if (m_onAtMaxDepth)
  {
    m_onAtMaxDepth->Call(entered);
  }
}

} // JSON
} // xr
