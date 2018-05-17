#ifndef XR_JSONWRITER_HPP
#define XR_JSONWRITER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "json.hpp"
#include "XR/fundamentals.hpp"
#include <sstream>
#include <vector>

namespace XR
{
namespace JSON
{

//==============================================================================
///@brief Writes JSON to a stream.
class Writer
{
  XR_NONCOPY_DECL(Writer)

public:
  //
  enum  Type
  {
    INVALID = -1,
    OBJECT,
    ARRAY
  };

  enum  EscapedChar
  {
    EC_QUOT,
    EC_BACKSLASH,
    EC_SLASH,
    EC_BACKSPACE,
    EC_FORM_FEED,
    EC_NEWLINE,
    EC_CARRIAGE_RETURN,
    EC_TAB,
    kNumEscapedChars
  };

  // static
  static const char   kIndent;

  static const char* const kEscapeChars;
  static const char* const kEscapeSequences[kNumEscapedChars];

  // structors
  explicit Writer(uint32_t maxDepth = kMaxParseDepthDefault);

  // general
  void        SetLinebreaks(bool pref);
  void        SetIndents(bool pref);
  void        SetSpaceAfterColon(bool pref);
  void        SetAutoEscape(bool pref);

  Writer& Start(Type rootType);

  Writer& WriteValue(const char* key, const char* value);
  Writer& WriteValue(const char* key, const int32_t value);
  Writer& WriteValue(const char* key, const double value);
  Writer& WriteValue(const char* key, bool value);

  Writer& WriteObject(const char* key);
  Writer& WriteArray(const char* key);

  Writer& WriteArrayElement(const char* value);
  Writer& WriteArrayElement(int32_t value);
  Writer& WriteArrayElement(double value);
  Writer& WriteArrayElement(bool value);

  Writer& WriteArrayObject();
  Writer& WriteArrayArray();

  Writer& CloseScope();

  std::string Finish(bool force = false); // force to close all scopes

protected:
  // types
  struct Scope
  {
    Type  type;
    bool  isEmpty;
  };

  // internal
  void  _AddLinebreak();
  void  _AddIndent();
  void  _AddSpace();
  void  _PushScope(Type type);
  void  _WriteKey( const char* key );
  void  _WriteComma();
  void  _WriteStringValue( const char* value );
  void  _ProcessEscaped( const char* value );

  // data
  bool  m_allowLinebreaks = false;
  bool  m_allowIndents = false;
  bool  m_allowSpace = false;
  bool  m_autoEscapeString = false;

  uint32_t  m_depth = 0;
  std::vector<Scope>  m_scopes;

  std::ostringstream  m_stream;
};

} // JSON
} // XR

#endif // XR_JSONWRITER_HPP
