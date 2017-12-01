#ifndef XR_JSONWRITER_HPP
#define XR_JSONWRITER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include <sstream>
#include "json.hpp"
#include "XR/fundamentals.hpp"

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
  static const char* const karEscapeSub[kNumEscapedChars];

  // structors
  explicit Writer(int maxDepth = kMaxParseDepthDefault);
  ~Writer();

  // general
  void        SetLinebreaks(bool pref);
  void        SetIndents(bool pref);
  void        SetSpaceAfterColon(bool pref);
  void        SetAutoEscape(bool pref);

  Writer& Start(Type rootType);

  Writer& WriteValue(const char* pKey, const char* pValue);
  Writer& WriteValue(const char* pKey, const int32_t value);
  Writer& WriteValue(const char* pKey, const double value);
  Writer& WriteValue(const char* pKey, bool value);

  Writer& WriteObject(const char* pKey);
  Writer& WriteArray(const char* pKey);

  Writer& WriteArrayElement(const char* pValue);
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
  void  _WriteKey( const char* pKey );
  void  _WriteComma();
  void  _WriteStringValue( const char* pValue );
  void  _ProcessEscaped( const char* pValue );

  // data
  bool                m_allowLinebreaks;
  bool                m_allowIndents;
  bool                m_allowSpace;
  bool                m_autoEscapeString;

  std::ostringstream  m_stream;  // no ownership

  Scope*              m_parScopes;
  int                 m_maxDepth;
  int                 m_depth;
};

} // JSON
} // XR

#endif // XR_JSONWRITER_HPP