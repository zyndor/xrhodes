//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/JsonWriter.hpp"
#include "xr/debug.hpp"
#include <cstring>
#include <algorithm>

namespace xr
{
namespace JSON
{

//==============================================================================
const char Writer::kEscapeChars[] = { '\b', '\t', '\n', '\f', '\r', '"', '/', '\\' };

const char* const Writer::kEscapeSequences[] =
{
  "\\b",
  "\\t"
  "\\n",
  "\\f",
  "\\r",
  "\\\"",
  "\\/",
  "\\\\",
};

//==============================================================================
Writer::Writer(std::ostream& stream, uint32_t maxDepth)
: m_allowLinebreaks(false),
  m_allowSpace(false),
  m_autoEscapeString(false),
  m_isComplete(false),
  m_stream(&stream)
{
  m_scopes.reserve(maxDepth);
}

//==============================================================================
Writer::~Writer()
{
  XR_ASSERTMSG(Writer, m_scopes.empty(),
    ("Scopes have remained open; the resulting JSON is likely invalid."));
}

//==============================================================================
void Writer::SetLinebreaks( bool pref )
{
  m_allowLinebreaks = pref;
}

//==============================================================================
void Writer::SetIndent(const char* indent)
{
  m_indent = indent ? indent : "";
}

//==============================================================================
void Writer::SetSpaces( bool pref )
{
  m_allowSpace = pref;
}

//==============================================================================
void Writer::SetAutoEscape( bool pref )
{
  m_autoEscapeString = pref;
}

//==============================================================================
Writer::Array Writer::OpenArray(bool oneLiner)
{
  return Array(WriteArray(oneLiner));
}

//==============================================================================
Writer::Object Writer::OpenObject(bool oneLiner)
{
  return Object(WriteObject(oneLiner));
}

//==============================================================================
Writer::Array Writer::OpenArray(char const * key, bool oneLiner)
{
  return Array(WriteArray(key, oneLiner));
}

//==============================================================================
Writer::Object Writer::OpenObject(char const * key, bool oneLiner)
{
  return Object(WriteObject(key, oneLiner));
}

//==============================================================================
Writer&  Writer::WriteValue( const char* value )
{
  XR_ASSERT(Json::Write, !m_isComplete);
  _WriteComma();
  _WriteStringValue(value);

  return *this;
}

//==============================================================================
Writer&  Writer::WriteValue( int32_t value )
{
  XR_ASSERT(Json::Write, !m_isComplete);
  _WriteComma();
  *m_stream << value;

  return *this;
}

//==============================================================================
Writer&  Writer::WriteValue( double value )
{
  XR_ASSERT(Json::Write, !m_isComplete);
  _WriteComma();
  *m_stream << value;

  return *this;
}

//==============================================================================
Writer&  Writer::WriteValue( bool value )
{
  XR_ASSERT(Json::Write, !m_isComplete);
  _WriteComma();
  *m_stream << std::boolalpha << value << std::noboolalpha;

  return *this;
}

//==============================================================================
Writer& Writer::WriteObject(bool oneLiner)
{
  XR_ASSERT(Json::Write, !m_isComplete);
  XR_ASSERTMSG(Json::Writer, m_scopes.empty() || m_scopes.back().type == Scope::ARRAY,
    ("Current scope is not root or an array; anonymous objects are invalid. Specify a key."));
  _WriteComma();
  _PushScope(Scope::OBJECT, oneLiner);

  return *this;
}

//==============================================================================
Writer& Writer::WriteArray(bool oneLiner)
{
  XR_ASSERT(Json::Write, !m_isComplete);
  XR_ASSERTMSG(Json::Writer, m_scopes.empty() || m_scopes.back().type == Scope::ARRAY,
    ("Current scope is not root or an array; anonymous arrays are invalid. Specify a key."));
  _WriteComma();
  _PushScope(Scope::ARRAY, oneLiner);

  return *this;
}

//==============================================================================
Writer&  Writer::WriteValue(const char* key, const char* value)
{
  XR_ASSERT(Json::Write, !m_isComplete);
  _WriteKey(key);

  if (!value)
  {
    *m_stream << kNull;
  }
  else
  {
    m_stream->put(kQuot);
    if (m_autoEscapeString)
    {
      _ProcessEscaped(value);
    }
    else
    {
      *m_stream << value;
    }
    m_stream->put(kQuot);
  }

  return *this;
}

//==============================================================================
Writer&  Writer::WriteValue(const char* key, const int32_t value)
{
  XR_ASSERT(Json::Write, !m_isComplete);
  _WriteKey(key);
  *m_stream << value;

  return *this;
}

//==============================================================================
Writer&  Writer::WriteValue(const char* key, const double value)
{
  XR_ASSERT(Json::Write, !m_isComplete);
  _WriteKey(key);
  *m_stream << value;

  return *this;
}

//==============================================================================
Writer&  Writer::WriteValue(const char* key, bool value)
{
  XR_ASSERT(Json::Write, !m_isComplete);
  _WriteKey(key);
  *m_stream << std::boolalpha << value << std::noboolalpha;

  return *this;
}

//==============================================================================
Writer& Writer::WriteObject(const char* key, bool oneLiner)
{
  XR_ASSERT(Json::Write, !m_isComplete);
  XR_ASSERTMSG(Json::Writer, m_scopes.back().type != Scope::ARRAY,
    ("Current scope is an array; a key must not be specified for a new object."));
  _WriteKey(key);
  _PushScope(Scope::OBJECT, oneLiner);

  return *this;
}

//==============================================================================
Writer& Writer::WriteArray(const char* key, bool oneLiner)
{
  XR_ASSERT(Json::Write, !m_isComplete);
  XR_ASSERTMSG(Json::Writer, m_scopes.back().type != Scope::ARRAY,
    ("Current scope is an array; a key must not be specified for a new object."));
  _WriteKey(key);
  _PushScope(Scope::ARRAY, oneLiner);

  return *this;
}

//==============================================================================
Writer&  Writer::CloseScope()
{
  XR_ASSERT(Json::Write, !m_isComplete);

  const Scope scope = m_scopes.back();
  m_scopes.pop_back();

  _AddLinebreak(scope.oneLiner);

  switch (scope.type)
  {
  case Scope::OBJECT:
    m_stream->put(kObjectEnd);
    break;

  case Scope::ARRAY:
    m_stream->put(kArrayEnd);
    break;

  default:
    break;
  }

  if (m_scopes.empty())
  {
    m_isComplete = true;  // no further writing should take place.
  }

  return *this;
}

//==============================================================================
void Writer::Reset(std::ostream & stream)
{
  m_stream = &stream;
  m_isComplete = false;
  m_scopes.clear();
}

//==============================================================================
void  Writer::_WriteKey( const char* key )
{
  XR_ASSERT(Json::Writer, key != nullptr);
  _WriteComma();

  m_stream->put(kQuot);
  if (m_autoEscapeString)
  {
    _ProcessEscaped(key);
  }
  else
  {
    *m_stream << key;
  }
  m_stream->put(kQuot).put(kColon);

  _AddSpace();
}

//==============================================================================
void  Writer::_WriteComma()
{
  if (!m_scopes.empty())
  {
    if (m_scopes.back().isEmpty)
    {
      m_scopes.back().isEmpty = false;
    }
    else
    {
      m_stream->put(kComma);
      _AddLinebreak(m_scopes.back().oneLiner);
    }
  }
}

//==============================================================================
static const char  kScopeOpeners[2] =
{
  kObjectBegin,
  kArrayBegin
};

void  Writer::_PushScope(Scope::Type type, bool oneLiner)
{
  m_scopes.push_back({ type, oneLiner, true });

  m_stream->put(kScopeOpeners[type]);
  _AddLinebreak(oneLiner);
}

//==============================================================================
void Writer::_AddLinebreak(bool oneLiner)
{
  if (m_allowLinebreaks && !oneLiner)
  {
    *m_stream << '\n';
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
  if (!m_indent.empty())
  {
    for (auto i = m_scopes.size(); i > 0; --i)
    {
      *m_stream << m_indent;
    }
  }
}

//==============================================================================
void Writer::_AddSpace()
{
  if (m_allowSpace)
  {
    m_stream->put(' ');
  }
}

//==============================================================================
void Writer::_WriteStringValue( const char* value )
{
  if (value == nullptr)
  {
    *m_stream << kNull;
  }
  else
  {
    ((m_stream->put(kQuot)) << value).put(kQuot);
  }
}

//==============================================================================
void Writer::_ProcessEscaped( const char* value )
{
  const char* end = strchr(value, '\0');
  while (value != end)
  {
    const char* nextEscape = strpbrk(value, kEscapeChars);
    bool  found = nextEscape != nullptr;
    if (!found)
    {
      nextEscape = end;
    }

    while (value != nextEscape)
    {
      *m_stream << *value;
      ++value;
    }

    if (found)
    {
      // process escape
      nextEscape = std::lower_bound(kEscapeChars, kEscapeChars + kNumEscapedChars, value[0]);
      if (nextEscape != kEscapeChars + kNumEscapedChars)
      {
        *m_stream << kEscapeSequences[nextEscape - kEscapeChars];
        ++value;
      }
    }
  }
}

//==============================================================================
Writer::ScopeWrapper::ScopeWrapper(Writer& writer)
: m_writer(&writer)
{}

//==============================================================================
Writer::ScopeWrapper::~ScopeWrapper()
{
  if (m_writer)
  {
    m_writer->CloseScope();
  }
}

//==============================================================================
Writer::Array::Array(Writer& writer)
: ScopeWrapper(writer)
{}

//==============================================================================
Writer::Array::Array(Array&& a)
: ScopeWrapper(*a.m_writer)
{
  a.m_writer = nullptr;
}

//==============================================================================
Writer::Array Writer::Array::OpenArray(bool oneLiner)
{
  return Array(m_writer->OpenArray(oneLiner));
}

//==============================================================================
Writer::Object Writer::Array::OpenObject(bool oneLiner)
{
  return Object(m_writer->WriteObject(oneLiner));
}

//==============================================================================
Writer::Object::Object(Writer& writer)
: ScopeWrapper(writer)
{}

//==============================================================================
Writer::Object::Object(Object && o)
: ScopeWrapper(*o.m_writer)
{
  o.m_writer = nullptr;
}

//==============================================================================
Writer::Array Writer::Object::OpenArray(char const* key, bool oneLiner)
{
  return Array(m_writer->OpenArray(key, oneLiner));
}

//==============================================================================
Writer::Object Writer::Object::OpenObject(char const* key, bool oneLiner)
{
  return Object(m_writer->WriteObject(key, oneLiner));
}

} // JSON
} // xr
