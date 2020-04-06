#ifndef XR_XONENTITY_HPP
#define XR_XONENTITY_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/types/fundamentals.hpp"
#include <map>
#include <vector>

namespace xr
{

class XonObject;
class XonValue;

//==============================================================================
///@brief Base class for XON entities.
class XonEntity
{
  XR_NONCOPY_DECL(XonEntity)

public:
  // types
  enum class Type
  {
    Object,
    Value,
  };

  ///@brief Exception type thrown for attempting to access functionality
  /// invalid for a given concrete XON object, i.e. trying to get elements
  /// of a Value or the value of an Object.
  struct Exception : public std::runtime_error
  {
    // types
    enum class Type
    {
      InvalidType,
      IndexOutOfBounds,
      InvalidKey,
    };

    // data
    Type const type;

    // structors
    Exception(Type type, std::string const& message);
  };

  // structors
  virtual ~XonEntity();

  // general
  Type GetType() const
  {
    return m_type;
  }

  ///@return Whether this is the given @a type.
  bool Is(Type type) const;

  ///@brief Attempts to cast this to a XonObject. A XonException of type InvalidType
  /// is thrown if this is not an object.
  XonObject& ToObject();

  ///@brief Attempts to cast this to a XonObject. A XonException of InvalidType
  /// is thrown if this is not an object.
  XonObject const& ToObject() const;

  ///@brief Attempts to cast this to a XonValue. A XonException of InvalidType
  /// is thrown if this is not a value.
  XonValue& ToValue();

  ///@brief Attempts to cast this to a XonValue. A XonException of InvalidType
  /// is thrown if this is not a value.
  XonValue const& ToValue() const;

protected:
  // structors
  explicit XonEntity(Type t);

private:
  // data
  Type m_type;
};

//==============================================================================
///@brief XON object - a container for other XON entities. Does not have a value
/// of its own. The elements may be indexed in the order of their declaration.
/// Additionally, elements declared with a key may be accessed using the key.
class XonObject : public XonEntity
{
public:
  // types
  using Xontainer = std::vector<XonEntity*>;

  // structors
  XonObject();
  ~XonObject();

  // general
  ///@return A pointer to the element mapped to the given @a key; nullptr if
  /// there is no such element.
  ///@note Does not transfer ownership.
  XonEntity*  TryGet(std::string const& key);

  ///@return A pointer to the element mapped to the given @a key; nullptr if
  /// there is no such element.
  XonEntity const*  TryGet(std::string const& key) const;

  ///@return A reference to the element mapped to the given @a key. A XonException
  /// of type InvalidKey is thrown if there is no such element.
  XonEntity&  Get(std::string const& key);

  ///@return A const reference to the element mapped to the given @a key. A
  /// XonException of type InvalidKey is thrown if there is no such element.
  XonEntity const&  Get(std::string const& key) const;

  ///@return The number of elements this object contains.
  size_t GetNumElements() const
  {
    return m_elements.size();
  }

  ///@brief Writes the keys of its elements into @a keys, in lexicographic order.
  void GetKeys(std::vector<std::string>& keys) const;

  ///@brief Adds the given element @a elem as a child at the next index.
  void AddElement(XonEntity& elem);  // ownership transfer

  ///@brief Adds the given element @a elem as a child at the next index, also
  /// keyed to @a key.
  ///@note Overrides a previous mapping of the same key (if any).
  void AddElement(std::string key, XonEntity& elem);  // ownership transfer

  ///@return Whether this has the given element @a elem.
  bool HasElement(XonEntity const& elem) const;

  // range based for support
  Xontainer::iterator begin() { return m_elements.begin(); }
  Xontainer::iterator end() { return m_elements.end(); }

  Xontainer::const_iterator begin() const { return m_elements.cbegin(); }
  Xontainer::const_iterator end() const { return m_elements.cend(); }

  // operators
  ///@return a referenec to the @a index th element this object has. A XonException
  /// of type IndexOutOfBounds will be thrown if index is outside of the range
  /// of valid indices.
  XonEntity&  operator[](size_t index);

  ///@return a const referenec to the @a index th element this object has. A
  /// XonException of type IndexOutOfBounds will be thrown if index is outside
  /// of the range of valid indices.
  XonEntity const&  operator[](size_t index) const;

private:
  // data
  Xontainer m_elements; // ownership
  std::map<std::string, XonEntity*> m_keyedElements;  // no ownership
};

//==============================================================================
///@brief A XON value, stored as a string.
class XonValue : public XonEntity
{
public:
  // structors
  XonValue();
  XonValue(const char* value, size_t length);
  ~XonValue();

  // general
  ///@brief returns the value of this element.
  const char* GetString() const
  {
    return m_value;
  }

private:
  // data
  char* m_value = nullptr;
};

} //XR

#endif //XR_XONENTITY_HPP
