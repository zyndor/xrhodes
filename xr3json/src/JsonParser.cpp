//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <XR/JsonParser.hpp>
#include <XR/debug.hpp>
#include <algorithm>
#include <limits>
#include <ctype.h>
#include <cstring>
#include <cmath>
#include <cstdio>

namespace XR
{
namespace JSON
{

//==============================================================================
Parser::Parser(int maxDepth)
: m_state(),
  m_depth(0),
  m_maxDepth(maxDepth),
  m_pCallback(nullptr),
  m_pCallbackUser(nullptr)
{
  XR_ASSERTMSG(Json::Parser, maxDepth > 1, ("%d is not a sensible value for maxDepth.",
    maxDepth));
}

//==============================================================================
Parser::~Parser ()
{}

//==============================================================================
bool  Parser::Parse(const char* parBuffer, int size, Callback pCallback,
        void* pUser)
{
  XR_ASSERT(Json::Parser, parBuffer != nullptr);
  XR_ASSERT(Json::Parser, size >= 0);
  m_state.SetBuffer(parBuffer, size);
  m_depth = 0;
  m_pCallback = pCallback;
  m_pCallbackUser = pUser;
  
  const char* pChar(m_state.ExpectChar()); // drop leading whitespace
  bool        result(!m_state.IsOver(pChar));
  if (result)
  {
    bool  isObject(*pChar == kObjectBegin);
    bool  isArray(*pChar == kArrayBegin);
    
    result = (isObject || isArray) && !m_state.IsOver(m_state.SkipChar());
    if (result) // check exceeding max depth (not an error)
    {
      result = isObject ? _ParseObject() : _ParseArray();
    }
  }
   
  return result;
}

//==============================================================================
bool  Parser::_ParseArray()
{
  _DoCallback(E_ARRAY_BEGIN, nullptr);
  _IncreaseDepth();
  const char* pChar(m_state.ExpectChar()); // look for array end or value
  bool result(!m_state.IsOver(pChar));
  if (result)
  {
    if (*pChar == kArrayEnd)
    {
      _DecreaseDepth();
      m_state.SkipChar();
    }
    else while (result)
    {
      result = _ParseValue();
      if (!result)
      {
        break;
      }

      pChar = m_state.ExpectChar();
      result = !m_state.IsOver(pChar) &&
        (*pChar == kComma || *pChar == kArrayEnd);
      if (!result)
      {
        break;
      }
      
      m_state.SkipChar(); // leave character
      if (*pChar == kArrayEnd)
      {
        _DecreaseDepth();
        break;
      }
    }
  }
  _DoCallback(E_ARRAY_END, nullptr);
  return result;
}

//==============================================================================
bool  Parser::_ParseObject()
{
  _DoCallback(E_OBJECT_BEGIN, nullptr);
  _IncreaseDepth();
  const char* pChar(m_state.ExpectChar());
  bool  result(!m_state.IsOver(pChar));
  if (result)
  {
    if (*pChar == kObjectEnd)
    {
      _DecreaseDepth();
      m_state.SkipChar();
    }
    else while (result)
    {
      pChar = m_state.ExpectChar();
      result = *pChar == kQuot;
      if (!result)
      {
        break;
      }

      const char* pKey(m_state.SkipChar()); // leave the quot
      result = !m_state.IsOver(pKey);
      if (!result)
      {
        break;
      }

      const char* pKeyEnd(m_state.RequireChar(kQuot)); // get other quot
      result = !m_state.IsOver(pKeyEnd);
      if (!result)
      {
        break;
      }
      
      XR_ASSERT(Json::Parser, pKeyEnd >= pKey);
      String  str = { pKey, size_t(pKeyEnd - pKey) };
      _DoCallback(E_KEY, &str);

      result = !m_state.IsOver(m_state.SkipChar());
      if (!result)
      {
        break;
      }

      pChar = m_state.ExpectChar();  // look for colon
      result = !m_state.IsOver(pChar) && *pChar == kColon;
      if (!result)
      {
        break;
      }

      result = !m_state.IsOver(m_state.SkipChar());
      if (!result)
      {
        break;
      }

      result = _ParseValue();
      if (!result)
      {
        break;
      }

      pChar = m_state.ExpectChar();
      result = !m_state.IsOver(pChar) &&
        (*pChar == kComma || *pChar == kObjectEnd);
      if (!result)
      {
        break;
      }

      m_state.SkipChar();
      if (*pChar == kObjectEnd)
      {
        _DecreaseDepth();
        break;
      }
    }
  }
  _DoCallback(E_OBJECT_END, nullptr);
  return result;
}

//==============================================================================
bool  Parser::_ParseValue()
{
  const char* pChar = m_state.ExpectChar();
  bool  result(!m_state.IsOver(pChar));
  if (result)
  {
    const int kBufferSize = 64; // bytes enough for everyone? (for conversion of numericals.)
    char  arBuffer[kBufferSize];

    if (*pChar == kQuot) // string
    {
      pChar = m_state.SkipChar();
      result = !m_state.IsOver(pChar);
      if (result)
      {
        const char* pValueEnd(m_state.RequireChar(kQuot)); // look for quot pair
        result = !m_state.IsOver(pValueEnd);
        if (result)
        {
          String  str = { pChar, size_t(pValueEnd - pChar) };
          _DoCallback(E_VALUE, &str);
          m_state.SkipChar();
        }
      }
    }
    else if (isdigit(*pChar) || *pChar == '-')
    {
      const char* pValueEnd(m_state.SkipChar());
      const char* pExponential = nullptr;
      const char* pDecimal = nullptr;
      while (result && !m_state.IsOver(pValueEnd) &&
        !(*pValueEnd == kComma || *pValueEnd == kObjectEnd || *pValueEnd == kArrayEnd ||
          isspace(*pValueEnd)))
      {
        const bool  isExponential(*pValueEnd == kExponential);
        const bool  isDecimal(*pValueEnd == kDecimal);
        result = isdigit(*pValueEnd) ||
          // first decimal point
          (*pValueEnd == kDecimal && pDecimal == nullptr) ||
          // first exponential, not straight after a minus sign
          (isExponential && pExponential == nullptr && *(pValueEnd - 1) != '-') ||
          // minus sign following an exponential
          (*pValueEnd == '-' && pExponential == pValueEnd - 1);
        if(result)
        {
          if(isDecimal)
          {
            pDecimal = pValueEnd;
          }
          else if(isExponential)
          {
            pExponential = pValueEnd;
          }

          pValueEnd = m_state.SkipChar();
          result = !m_state.IsOver(pValueEnd);
        }
      }

      if (result)
      {
        result = !m_state.IsOver(pValueEnd);
      }

      if (result)
      {
        int len = pValueEnd - pChar;
        
        double  value(atof(pChar));
        double  absValue(std::abs(value));
        if(absValue - std::floor(absValue) < std::numeric_limits<double>::epsilon())
        {
          len = std::snprintf(arBuffer, kBufferSize - 1, "%d", int(value));
        }
        else
        {
          len = std::min(kBufferSize - 1, len);
          std::strncpy(arBuffer, pChar, len);
          arBuffer[len] = '\0';
        }
        
        String  str = { arBuffer, len };
        _DoCallback(E_VALUE, &str);
      }
    }
    else if (isalpha(*pChar))
    {
      const char* pValueEnd(m_state.SkipChar());
      while (!m_state.IsOver(pValueEnd) &&
        !(*pValueEnd == kComma || *pValueEnd == kObjectEnd || *pValueEnd == kArrayEnd ||
          isspace(*pValueEnd)))
      {
        pValueEnd = m_state.SkipChar();
      }

      const int len = pValueEnd - pChar;
      if (strncmp(pChar, kFalse, len) == 0)
      {
        std::snprintf(arBuffer, kBufferSize - 1, "0");
        String  str = { arBuffer, strlen(arBuffer) };
        _DoCallback(E_VALUE, &str);
      }
      else if (strncmp(pChar, kTrue, len) == 0)
      {
        std::snprintf(arBuffer, kBufferSize - 1, "1");
        String  str = { arBuffer, strlen(arBuffer) };
        _DoCallback(E_VALUE, &str);
      }
      else if (strncmp(pChar, kNull, len) == 0)
      {
        String  str = { nullptr, 0 };
        _DoCallback(E_VALUE, &str);
      }
      else
      {
        result = false;
      }
    }
    else if (*pChar == kObjectBegin)
    {
      if (!m_state.IsOver(m_state.SkipChar()))
      {
        result = _ParseObject();
      }
    }
    else if (*pChar == kArrayBegin)
    {
      if (!m_state.IsOver(m_state.SkipChar()))
      {
        result = _ParseArray();
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
void  Parser::_DoCallback(Event e, const String* pData)
{
  if (m_pCallback != nullptr && m_depth <= m_maxDepth)
  {
    (*m_pCallback)(e, pData, m_pCallbackUser);
  }
}

//==============================================================================
void Parser::_IncreaseDepth()
{
  ++m_depth;
  if (m_depth == m_maxDepth + 1)
  {
    _DoDepthCallback(true);
  }
}

//==============================================================================
void Parser::_DecreaseDepth()
{
  --m_depth;
  if (m_depth == m_maxDepth)
  {
    _DoDepthCallback(false);
  }
}

//==============================================================================
void Parser::_DoDepthCallback(bool entered)
{
  if (m_pMaxDepthCallback != nullptr)
  {
    (*m_pMaxDepthCallback)(&entered, m_pMaxDepthUserData);
  }
}

} // JSON
} // XR
