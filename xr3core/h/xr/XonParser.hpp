#ifndef XR_XONPARSER_HPP
#define XR_XONPARSER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================

#include <cstddef>

namespace xr
{

class ParserCore;

//==============================================================================
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
    const char* start;
    size_t      length;
    bool        isQuoted;
  };

  ///@brief Event handling callback.
  ///@return Whether the parsing should continue. This provides an opportunity
  /// to break on semantic errors.
  typedef bool(*Callback)(Event e, String const* data, void* userData);

  struct State
  {
    int row;
    int column;
    int depth;
    const char* cursor;
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

  bool Parse(char const* string, size_t length, Callback callback, void* userData);

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
  Callback    m_callback;
  void*       m_callbackUser;

  // internal
  bool          ParseObject(ParserCore& pc);
  ElementResult ParseElement(ParserCore& pc, String& strOut);
  bool          ParseString(ParserCore& pc, String& strOut);
  const char*   ProcessComments(ParserCore& pc);
  bool          DoCallback(Event e, String const* string);
};

//==============================================================================
// implementation
//==============================================================================
inline
XonParser::State const& XonParser::GetState() const
{
  return m_state;
}

} // xr

#endif //XR_XONPARSER_HPP
