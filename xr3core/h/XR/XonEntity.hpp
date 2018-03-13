#ifndef XR_XONENTITY_HPP
#define XR_XONENTITY_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "fundamentals.hpp"
#include <map>
#include <vector>

namespace XR
{

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

  virtual XonEntity&        operator[](size_t index);
  virtual XonEntity const&  operator[](size_t index) const;

  virtual XonEntity&        Find(std::string const& name);
  virtual XonEntity const&  Find(std::string const& name) const;

  virtual const char* GetValue() const;

  virtual size_t GetNumElements() const;

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
  // structors
  XonObject();
  ~XonObject();

  // general
  virtual XonEntity&        operator[](size_t index) override;
  virtual XonEntity const&  operator[](size_t index) const override;

  virtual XonEntity&        Find(std::string const& name) override;
  virtual XonEntity const&  Find(std::string const& name) const override;

  virtual size_t GetNumElements() const override
  {
    return m_values.size();
  }

  ///@brief Writes the keys that it knows of into @a keys, in lexicographic order.
  void GetKeys(std::vector<std::string>& keys);

  void AddValue(XonEntity& value);  // ownership transfer
  void AddValue(std::string key, XonEntity& value);  // ownership transfer
  bool HasValue(XonEntity const& value) const;

private:
  // data
  std::vector<XonEntity*>           m_values; // ownership
  std::map<std::string, XonEntity*> m_keyValues;  // no ownership
};

//==============================================================================
///@brief A XON value, stored as a string.
class XonValue : public XonEntity
{
public:
  // structors
  XonValue();
  XonValue(const char* pValue, size_t length);
  ~XonValue();

  // general
  virtual const char* GetValue() const override
  {
    return m_value;
  }

private:
  // data
  char* m_value = nullptr;
};

} //XR

#endif //XR_XONENTITY_HPP
