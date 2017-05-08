//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "JsonWriter.hpp"
#include <XR/debug.hpp>
#include <cstring>

namespace XR
{
namespace JSON
{

//==============================================================================
const char  Writer::kIndent = '\t';

const char* const Writer::kEscapeChars = "\"\\/\b\f\n\r\t";
const char* const Writer::karEscapeSub[] =
{
  "\\\"",
  "\\\\",
  "\\/",
  "\\b",
  "\\f",
  "\\n",
  "\\r",
  "\\t"
};

//==============================================================================
Writer::Writer(int maxDepth)
: m_stream(),
  m_parScopes(nullptr),
  m_maxDepth(maxDepth),
  m_depth(0),
  m_allowLinebreaks(false),
  m_allowIndents(false),
  m_allowSpace(false),
  m_autoEscapeString(false)
{
  m_parScopes = new Scope[maxDepth];
}

//==============================================================================
Writer::~Writer()
{
  delete[] m_parScopes;
}

//==============================================================================
void Writer::SetLinebreaks( bool pref )
{
  m_allowLinebreaks = pref;
}

//==============================================================================
void Writer::SetIndents( bool pref )
{
  m_allowIndents = pref;
}

//==============================================================================
void Writer::SetSpaceAfterColon( bool pref )
{
  m_allowSpace = pref;
}

//==============================================================================
void Writer::SetAutoEscape( bool pref )
{
  m_autoEscapeString = pref;
}

//==============================================================================
Writer&  Writer::Start(Type rootType)
{
  XR_ASSERT(Json::Writer, rootType != INVALID);
  m_stream.str("");

  _PushScope(rootType);

  return *this;
}

//==============================================================================
Writer&  Writer::WriteValue( const char* pKey, const char* pValue )
{
  _WriteKey(pKey);

  if (pValue == 0)
  {
    m_stream << kNull;
  }
  else
  {
    m_stream.put(kQuot);
    if (m_autoEscapeString)
    {
      _ProcessEscaped(pValue);
    }
    else
    {
      m_stream << pValue;
    }
    m_stream.put(kQuot);
  }

  return *this;
}

//==============================================================================
Writer&  Writer::WriteValue( const char* pKey, const int32_t value )
{
  _WriteKey(pKey);
  m_stream << value;

  return *this;
}

//==============================================================================
Writer&  Writer::WriteValue( const char* pKey, const double value )
{
  _WriteKey(pKey);
  m_stream << value;

  return *this;
}

//==============================================================================
Writer&  Writer::WriteValue( const char* pKey, bool value )
{
  _WriteKey(pKey);
  m_stream << std::boolalpha << value << std::noboolalpha;

  return *this;
}

//==============================================================================
Writer&  Writer::WriteObject( const char* pKey )
{
  XR_ASSERTMSG(Json::Writer, m_parScopes[m_depth].type != ARRAY,
    ("Current scope is an array, named objects are invalid. Use WriteArrayObject() instead."));
  _WriteKey(pKey);
  _PushScope(OBJECT);

  return *this;
}

//==============================================================================
Writer&  Writer::WriteArray( const char* pKey )
{
  XR_ASSERTMSG(Json::Writer, m_parScopes[m_depth].type != ARRAY,
    ("Current scope is an array, named arrays are invalid. Use WriteArrayArray() instead."));
  _WriteKey(pKey);
  _PushScope(ARRAY);

  return *this;
}

//==============================================================================
Writer&  Writer::WriteArrayElement( const char* pValue )
{
  _WriteComma();
  _WriteStringValue(pValue);

  return *this;
}

//==============================================================================
Writer&  Writer::WriteArrayElement( int32_t value )
{
  _WriteComma();
  m_stream << value;

  return *this;
}

//==============================================================================
Writer&  Writer::WriteArrayElement( double value )
{
  _WriteComma();
  m_stream << value;

  return *this;
}

//==============================================================================
Writer&  Writer::WriteArrayElement( bool value )
{
  _WriteComma();
  m_stream << std::boolalpha << value << std::noboolalpha;

  return *this;
}

//==============================================================================
Writer&  Writer::WriteArrayObject()
{
  XR_ASSERTMSG(Json::Writer, m_parScopes[m_depth].type == ARRAY,
    ("Current scope is not an array, anonymous objects are invalid. Use WriteArrayArray() instead."));
  _WriteComma();
  _PushScope(OBJECT);

  return *this;
}

//==============================================================================
Writer& Writer::WriteArrayArray()
{
  XR_ASSERTMSG(Json::Writer, m_parScopes[m_depth].type == ARRAY,
    ("Current scope is not an array, anonymous arrays are invalid. Use WriteArrayArray() instead."));
  _WriteComma();
  _PushScope(ARRAY);

  return *this;
}

//==============================================================================
Writer&  Writer::CloseScope()
{
  XR_ASSERT(Json::Writer, m_depth > 0);

  const int type(m_parScopes[m_depth].type);
  --m_depth;

  _AddLinebreak();
  switch (type)
  {
  case OBJECT:
    m_stream.put(kObjectEnd);
    break;
  
  case ARRAY:
    m_stream.put(kArrayEnd);
    break;
  }

  return *this;
}

//==============================================================================
std::string Writer::Finish( bool force )
{
  if (force)
  {
    while (m_depth > 0)
    {
      CloseScope();
    }
  }

  XR_ASSERTMSG(Json::Writer, m_depth == 0,
    ("CloseScope() needs to be called for %d scopes (or call Finish(true)).",
    m_depth));
  
  return m_stream.str();
}

//==============================================================================
void  Writer::_WriteKey( const char* pKey )
{
  XR_ASSERT(MG, pKey != nullptr);
  _WriteComma();

  m_stream.put(kQuot);
  if (m_autoEscapeString)
  {
    _ProcessEscaped(pKey);  
  }
  else
  {
    m_stream << pKey;
  }
  m_stream.put(kQuot).put(kColon);

  _AddSpace();
}

//==============================================================================
void  Writer::_WriteComma()
{
  XR_ASSERT(Json::Writer, m_depth > 0);
  if (m_parScopes[m_depth].isEmpty)
  {
    m_parScopes[m_depth].isEmpty = false;
  }
  else
  {
    m_stream.put(kComma);
    _AddLinebreak();
  }
}

//==============================================================================
static const char  karScopeBegin[2] =
{
  kObjectBegin,
  kArrayBegin
};

void  Writer::_PushScope(Type type)
{
  XR_ASSERT(Json::Writer, type > INVALID);
  XR_ASSERT(Json::Writer, type <= ARRAY);
  XR_ASSERT(Json::Writer, m_depth + 1 < m_maxDepth);
  ++m_depth;
  m_parScopes[m_depth].type = type;
  m_parScopes[m_depth].isEmpty = true;

  m_stream.put(karScopeBegin[type]);
  _AddLinebreak();
}

//==============================================================================
void Writer::_AddLinebreak()
{
  if (m_allowLinebreaks)
  {
    m_stream << '\n';
    _AddIndent();
  }
  else
  {
    _AddSpace();
  }
}

//==============================================================================
void Writer::_AddIndent()
{
  if (m_allowIndents)
  {
    for (int i = m_depth; i > 0; --i)
    {
      m_stream.put(kIndent);
    }
  }
}

//==============================================================================
void Writer::_AddSpace()
{
  if (m_allowSpace)
  {
    m_stream.put(' ');
  }
}

//==============================================================================
void Writer::_WriteStringValue( const char* pValue )
{
  if (pValue == nullptr)
  {
    m_stream << kNull;
  }
  else
  {
    ((m_stream.put(kQuot)) << pValue).put(kQuot);
  }
}

//==============================================================================
void Writer::_ProcessEscaped( const char* pValue )
{
  const char* pEnd(strchr(pValue, '\0'));
  while (pValue != pEnd)
  {
    const char* pNextEscape = strpbrk(pValue, kEscapeChars);
    bool  found(pNextEscape != nullptr);
    if (!found)
    {
      pNextEscape = pEnd;
    }

    while (pValue != pNextEscape)
    {
      m_stream << *pValue;
      ++pValue;
    }

    if (found)
    {
      // process escape
      pNextEscape = strchr(kEscapeChars, pValue[0]);
      if (pNextEscape != nullptr)
      {
        m_stream << karEscapeSub[pNextEscape - kEscapeChars];
        ++pValue;
      }
    }
  }  
}

} // JSON
} // XR
