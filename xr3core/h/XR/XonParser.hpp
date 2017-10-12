//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_XONPARSER_HPP
#define XR_XONPARSER_HPP

#include <cstddef>

namespace XR
{

class ParserCore;

///@brief XRhodes Object Notation parser. Summary of the XON format:
/// - Inspired by Lua (tables) and JSON.
/// - Entities can be objects or values,
/// - Objects are arrays of values,
/// - Objects are also maps of key-value pairs,
/// - The only string[ value]s that need to be quoted are empty strings, those
///   containing whitespace and/or reserved characters,
/// - Supports line comments,
/// - Trailing commas in lists of values are forgiven, unlike in json.
///@note:
/// This is a syntax parser only; as such it may be used to implement fairly
/// different formats / languages. It is not concerned with semantics related
/// matters such as null values and element storage and access.
class XonParser
{
public:
  // types
  enum class Event
  {
    ObjectBegin,
    Key,
    Value,
    ObjectEnd,
  };

  struct String
  {
    const char* pStart;
    size_t      length;
    bool        isQuoted;
  };

  ///@brief Event handling callback.
  ///@return Whether the parsing should continue. This provides an opportunity
  /// to break on semantic errors.
  typedef bool(*Callback)(Event e, String const* pData, void* pUser);

  struct State
  {
    int row;
    int column;
    int depth;
    const char* pCurrent;
  };

  // static
  static char const kObjectBegin = '{';
  static char const kObjectEnd = '}';
  static char const kComment = '#'; // Rest of the line.
  static char const kKey = ':';
  static char const kNextValue = ',';
  static char const kQuot = '"';
  static char const kEscape = '\\'; // used to escape quotation marks in a quoted string only.

  // general
  State const& GetState() const;

  bool Parse(char const* pBuffer, size_t length, Callback pCallback, void* pUser);

private:
  // types
  struct ElementResult
  {
    bool isValid;

    enum class Type
    {
      String,
      Object
    } type;
  };

  // data
  State       m_state;
  Callback    m_pCallback;
  void*       m_pCallbackUser;

  // internal
  bool          ParseObject(ParserCore& pc);
  ElementResult ParseElement(ParserCore& pc, String& strOut);
  bool          ParseString(ParserCore& pc, String& strOut);
  const char*   ProcessComments(ParserCore& pc);
  bool          DoCallback(Event e, String const* pString);
};

//==============================================================================
// implementation
//==============================================================================
inline
XonParser::State const& XonParser::GetState() const
{
  return m_state;
}

} // XR

#endif //XR_XONPARSER_HPP
