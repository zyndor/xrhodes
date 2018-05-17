//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/JsonWriter.hpp"
#include "XR/debug.hpp"
#include <cstring>

namespace XR
{
namespace JSON
{

//==============================================================================
const char  Writer::kIndent = '\t';

const char* const Writer::kEscapeChars = "\"\\/\b\f\n\r\t";
const char* const Writer::kEscapeSequences[] =
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
Writer::Writer(uint32_t maxDepth)
: m_scopes(maxDepth)
{}

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
Writer&  Writer::WriteValue( const char* key, const char* value )
{
  _WriteKey(key);

  if (value == 0)
  {
    m_stream << kNull;
  }
  else
  {
    m_stream.put(kQuot);
    if (m_autoEscapeString)
    {
      _ProcessEscaped(value);
    }
    else
    {
      m_stream << value;
    }
    m_stream.put(kQuot);
  }

  return *this;
}

//==============================================================================
Writer&  Writer::WriteValue( const char* key, const int32_t value )
{
  _WriteKey(key);
  m_stream << value;

  return *this;
}

//==============================================================================
Writer&  Writer::WriteValue( const char* key, const double value )
{
  _WriteKey(key);
  m_stream << value;

  return *this;
}

//==============================================================================
Writer&  Writer::WriteValue( const char* key, bool value )
{
  _WriteKey(key);
  m_stream << std::boolalpha << value << std::noboolalpha;

  return *this;
}

//==============================================================================
Writer&  Writer::WriteObject( const char* key )
{
  XR_ASSERTMSG(Json::Writer, m_scopes[m_depth].type != ARRAY,
    ("Current scope is an array, named objects are invalid. Use WriteArrayObject() instead."));
  _WriteKey(key);
  _PushScope(OBJECT);

  return *this;
}

//==============================================================================
Writer&  Writer::WriteArray( const char* key )
{
  XR_ASSERTMSG(Json::Writer, m_scopes[m_depth].type != ARRAY,
    ("Current scope is an array, named arrays are invalid. Use WriteArrayArray() instead."));
  _WriteKey(key);
  _PushScope(ARRAY);

  return *this;
}

//==============================================================================
Writer&  Writer::WriteArrayElement( const char* value )
{
  _WriteComma();
  _WriteStringValue(value);

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
  XR_ASSERTMSG(Json::Writer, m_scopes[m_depth].type == ARRAY,
    ("Current scope is not an array; anonymous objects are invalid. Use WriteArrayArray() instead."));
  _WriteComma();
  _PushScope(OBJECT);

  return *this;
}

//==============================================================================
Writer& Writer::WriteArrayArray()
{
  XR_ASSERTMSG(Json::Writer, m_scopes[m_depth].type == ARRAY,
    ("Current scope is not an array; anonymous arrays are invalid. Use WriteArrayArray() instead."));
  _WriteComma();
  _PushScope(ARRAY);

  return *this;
}

//==============================================================================
Writer&  Writer::CloseScope()
{
  XR_ASSERT(Json::Writer, m_depth > 0);

  const Type type(m_scopes[m_depth].type);
  XR_ASSERT(Json::Writer, type != INVALID);
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

  default:
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
void  Writer::_WriteKey( const char* key )
{
  XR_ASSERT(MG, key != nullptr);
  _WriteComma();

  m_stream.put(kQuot);
  if (m_autoEscapeString)
  {
    _ProcessEscaped(key);
  }
  else
  {
    m_stream << key;
  }
  m_stream.put(kQuot).put(kColon);

  _AddSpace();
}

//==============================================================================
void  Writer::_WriteComma()
{
  XR_ASSERT(Json::Writer, m_depth > 0);
  if (m_scopes[m_depth].isEmpty)
  {
    m_scopes[m_depth].isEmpty = false;
  }
  else
  {
    m_stream.put(kComma);
    _AddLinebreak();
  }
}

//==============================================================================
static const char  kScopeOpeners[2] =
{
  kObjectBegin,
  kArrayBegin
};

void  Writer::_PushScope(Type type)
{
  XR_ASSERT(Json::Writer, type == OBJECT || type == ARRAY);
  XR_ASSERT(Json::Writer, m_depth + 1 < m_scopes.size());
  ++m_depth;
  m_scopes[m_depth].type = type;
  m_scopes[m_depth].isEmpty = true;

  m_stream.put(kScopeOpeners[type]);
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
void Writer::_WriteStringValue( const char* value )
{
  if (value == nullptr)
  {
    m_stream << kNull;
  }
  else
  {
    ((m_stream.put(kQuot)) << value).put(kQuot);
  }
}

//==============================================================================
void Writer::_ProcessEscaped( const char* value )
{
  const char* end = strchr(value, '\0');
  while (value != end)
  {
    const char* nextEscape = strpbrk(value, kEscapeChars);
    bool  found(nextEscape != nullptr);
    if (!found)
    {
      nextEscape = end;
    }

    while (value != nextEscape)
    {
      m_stream << *value;
      ++value;
    }

    if (found)
    {
      // process escape
      nextEscape = strchr(kEscapeChars, value[0]);
      if (nextEscape != nullptr)
      {
        m_stream << kEscapeSequences[nextEscape - kEscapeChars];
        ++value;
      }
    }
  }
}

} // JSON
} // XR
