#ifndef XR_JSONWRITER_HPP
#define XR_JSONWRITER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "json.hpp"
#include "xr/fundamentals.hpp"
#include <sstream>
#include <vector>

namespace xr
{
namespace JSON
{

//==============================================================================
///@brief Facilitates the writing of a JSON document, to a stream, allowing a
/// variety of formatting options for indenting, line breaks etc.
class Writer
{
  XR_NONCOPY_DECL(Writer)

  ///@brief Wraps a Writer for a scope; calls CloseScope() upon destruction.
  struct ScopeWrapper
  {
    ScopeWrapper(Writer& writer);
    ~ScopeWrapper();

  protected:
    Writer* m_writer;
  };

public:
  // types
  enum  EscapedChar
  {
    EC_BACKSPACE,
    EC_TAB,
    EC_NEWLINE,
    EC_FORM_FEED,
    EC_CARRIAGE_RETURN,
    EC_QUOT,
    EC_SLASH,
    EC_BACKSLASH,
    kNumEscapedChars
  };

  struct Object;

  ///@brief Manages a JSON Array scope, closing it upon destruction.
  struct Array: public ScopeWrapper
  {
    Array(Array&& a);

    template <typename T>
    Array& WriteValue(T const& value)
    {
      m_writer->WriteValue(value);
      return *this;
    }

    ///@brief Opens an array scope. If @a oneLiner is true, no line breaks will be
    /// written after child values.
    ///@return An Array scope object which will close itself upon going out of scope.
    Array OpenArray(bool oneLiner = false);

    ///@brief Opens an object scope. If @a oneLiner is true, no line breaks will be
    /// written after child values.
    ///@return An Array scope object which will close itself upon going out of scope.
    Object OpenObject(bool oneLiner = false);

    // friends
    friend class Writer;

  private:
    explicit Array(Writer& writer);
  };

  ///@brief Manages a JSON Object scope, closing it upon destruction.
  struct Object : public ScopeWrapper
  {
    Object(Object&& o);

    template <typename T>
    Object& WriteValue(char const* key, T const& value)
    {
      m_writer->WriteValue(key, value);
      return *this;
    }

    ///@brief Opens an array scope for the given @a key. If @a oneLiner is true,
    /// no line breaks will be written after child values.
    ///@return An Array scope object which will close itself upon going out of scope.
    Array OpenArray(char const* key, bool oneLiner = false);

    ///@brief Opens an object scope for the given @a key. If @a oneLiner is true,
    /// no line breaks will be written after child values.
    ///@return An Array scope object which will close itself upon going out of scope.
    Object OpenObject(char const* key, bool oneLiner = false);

    // friends
    friend class Writer;

  private:
    explicit Object(Writer& writer);
  };

  // static
  static const char kEscapeChars[kNumEscapedChars];
  static const char* const kEscapeSequences[kNumEscapedChars];

  // structors
  ///@brief Initialises The Writer to use the given @a stream and reserves
  /// space for @a maxDepth scopes on the stack.
  explicit Writer(std::ostream& stream, uint32_t maxDepth = kMaxParseDepthDefault);
  ~Writer();

  // general
  ///@brief Sets the @a indent written before new entities, as many times
  /// as many objects / arrays are open. Passing nullptr or empty string
  /// disables indenting.
  void  SetIndent(const char* indent);

  ///@brief Sets whether a line break should be written after every element.
  /// If disabled, a space may be written if SetSpaces(true) was called.
  ///@note The oneLiner parameter of Write{Array|Object}() may apply a negative
  /// override to it, i.e. suppress it if enabled.
  void  SetLinebreaks(bool pref);

  ///@brief Sets whether spaces are written after colons and commas, etc..
  void  SetSpaces(bool pref);

  ///@brief Sets whether special characters in keys / values are automatically
  /// escaped. (See EscapedChars for listing of these.)
  void  SetAutoEscape(bool pref);

  ///@brief Opens an array scope. If @a oneLiner is true, no line breaks will be
  /// written after child values.
  ///@return An Array scope object which will close itself upon going out of scope.
  ///@note The Writer must either be at the root element or in an array scope.
  Array OpenArray(bool oneLiner = false);

  ///@brief Opens an object scope. If @a oneLiner is true, no line breaks will be
  /// written after child values.
  ///@return An Array scope object which will close itself upon going out of scope.
  ///@note The Writer must either be at the root element or in an array scope.
  Object OpenObject(bool oneLiner = false);

  ///@brief Opens an array scope for the given @a key. If @a oneLiner is true,
  /// no line breaks will be written after child values.
  ///@return An Array scope object which will close itself upon going out of scope.
  ///@note There must be a current scope and it must be of an object.
  Array OpenArray(char const* key, bool oneLiner = false);

  ///@brief Opens an object scope. If @a oneLiner is true, no line breaks will be
  /// written after child values.
  ///@return An Array scope object which will close itself upon going out of scope.
  ///@note There must be a current scope and it must be of an object.
  Object OpenObject(char const* key, bool oneLiner = false);

  ///@brief Writes a @a value.
  ///@note There must be a current scope and it must be of an array.
  Writer& WriteValue(const char* value);

  ///@brief Writes a @a value.
  ///@note There must be a current scope and it must be of an array.
  Writer& WriteValue(int32_t value);

  ///@brief Writes a @a value.
  ///@note There must be a current scope and it must be of an array.
  Writer& WriteValue(double value);

  ///@brief Writes a @a value.
  ///@note There must be a current scope and it must be of an array.
  Writer& WriteValue(bool value);

  ///@brief Opens an object. If @a oneLiner is true, no line breaks will be
  /// written after child values.
  ///@note Nested arrays and objects will write line breaks unless created with
  /// oneLiner == true themselves.
  ///@note The Writer must either be at the root element or in an array scope.
  Writer& WriteObject(bool oneLiner = false);

  ///@brief Opens an object. If @a oneLiner is true, no line breaks will be
  /// written after child values.
  ///@note Nested arrays and objects will write line breaks unless created with
  /// oneLiner == true themselves.
  ///@note The Writer must either be at the root element or in an array scope.
  Writer& WriteArray(bool oneLiner = false);

  ///@brief Writes a string @a value to the given @a key.
  ///@note There must be a current scope and it must be of an object.
  Writer& WriteValue(const char* key, const char* value);

  ///@brief Writes an integer @a value to the given @a key.
  ///@note There must be a current scope and it must be of an object.
  Writer& WriteValue(const char* key, const int32_t value);

  ///@brief Writes a double @a value to the given @a key.
  ///@note There must be a current scope and it must be of an object.
  Writer& WriteValue(const char* key, const double value);

  ///@brief Writes a boolean @a value to the given @a key.
  ///@note There must be a current scope and it must be of an object.
  Writer& WriteValue(const char* key, bool value);

  ///@brief Opens an object with the given @a key. If @a oneLiner is true, no
  /// line breaks will be written after child values.
  ///@note Nested arrays and objects will write line breaks unless created with
  /// oneLiner == true themselves.
  ///@note There must be a current scope and it must be of an object.
  Writer& WriteObject(const char* key, bool oneLiner = false);

  ///@brief Opens an array with the given @a key. If @a oneLiner is true, no
  /// line breaks will be written after child values.
  ///@note Nested arrays and objects will write line breaks unless created with
  /// oneLiner == true themselves.
  ///@note There must be a current scope and it must be of an object.
  Writer& WriteArray(const char* key, bool oneLiner = false);

  ///@brief Closes the current scope. If this was the root object, the document
  /// will be marked complete, which means that any further attempts to write
  /// to it will trigger an assert in debug builds and the result will be
  /// malformed json in all builds.
  Writer& CloseScope();

  ///@brief Clears the stack, the completion flag and reassigns the stream,
  /// allowing The Writer to be reused.
  void Reset(std::ostream& stream);

protected:
  // types
  struct Scope
  {
    enum  Type
    {
      OBJECT,
      ARRAY
    };

    Type  type : 2;
    bool  oneLiner : 1;
    bool  isEmpty : 1;
  };

  // data
  std::string m_indent;
  bool  m_allowLinebreaks : 1;
  bool  m_allowSpace : 1;
  bool  m_autoEscapeString : 1;
  bool  m_isComplete : 1;

  std::ostream* m_stream;
  std::vector<Scope>  m_scopes;

  // internal
  void  _AddLinebreak(bool oneLiner);
  void  _AddIndent();
  void  _AddSpace();
  void  _PushScope(Scope::Type type, bool oneLiner);
  void  _WriteKey(const char* key);
  void  _WriteComma();
  void  _WriteStringValue(const char* value);
  void  _ProcessEscaped(const char* value);
};

} // JSON
} // xr

#endif // XR_JSONWRITER_HPP
