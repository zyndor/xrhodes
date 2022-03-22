//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/xon/XonEntity.hpp"
#include <algorithm>
#include <string>
#include <cstring>

namespace xr
{
namespace
{
//==============================================================================
void ThrowInvalidTypeError(char const* operationName)
{
  char msgBuffer[128];
  sprintf(msgBuffer, "Invalid type: %s not supported.", operationName);
  throw XonEntity::Exception(XonEntity::Exception::Type::InvalidType, msgBuffer);
}

//==============================================================================
void ThrowIndexOutOfBoundsError(size_t index, size_t bounds)
{
  char msgBuffer[256];
  sprintf(msgBuffer, "Index (%zd) out of bounds (%zd).", index, bounds);
  throw XonEntity::Exception(XonEntity::Exception::Type::IndexOutOfBounds, msgBuffer);
}

//==============================================================================
void ThrowInvalidKeyError(const char* name)
{
  char msgBuffer[128];
  auto len = strlen(name);
  const size_t kMaxSize = 100;
  bool overSized = len > kMaxSize;
  if (overSized)
  {
    name += len - kMaxSize;
  }
  sprintf(msgBuffer, "Invalid key '%s%s'.", overSized ? "..." : "", name);
  throw XonEntity::Exception(XonEntity::Exception::Type::InvalidKey, msgBuffer);
}

} //

//==============================================================================
XonEntity::Exception::Exception(Type t, std::string const & message)
: std::runtime_error(message.c_str()),
  type(t)
{}

//==============================================================================
XonEntity::XonEntity() = default;
XonEntity::~XonEntity() = default;

//==============================================================================
bool XonEntity::Is(Type type) const
{
  return GetType() == type;
}

//==============================================================================
XonObject& XonEntity::ToObject()
{
  if (GetType() != Type::Object)
  {
    ThrowInvalidTypeError("ToObject");
  }
  return static_cast<XonObject&>(*this);
}

//==============================================================================
XonObject const& XonEntity::ToObject() const
{
  if (GetType() != Type::Object)
  {
    ThrowInvalidTypeError("ToObject");
  }
  return static_cast<XonObject const&>(*this);
}

//==============================================================================
XonValue& XonEntity::ToValue()
{
  if (GetType() != Type::Value)
  {
    ThrowInvalidTypeError("ToValue");
  }
  return static_cast<XonValue&>(*this);
}

//==============================================================================
XonValue const& XonEntity::ToValue() const
{
  if (GetType() != Type::Value)
  {
    ThrowInvalidTypeError("ToValue");
  }
  return static_cast<XonValue const&>(*this);
}

//==============================================================================
XonObject::XonObject() = default;
XonObject::~XonObject() = default;

//==============================================================================
XonEntity::Type XonObject::GetType() const
{
  return Type::Object;
}

//==============================================================================
XonEntity* XonObject::TryGet(std::string const& name)
{
  XonEntity* result = nullptr;
  auto iFind = m_keyedElements.find(name);
  if (iFind != m_keyedElements.end())
  {
    result = iFind->second;
  }
  return result;
}

//==============================================================================
XonEntity const* XonObject::TryGet(std::string const& name) const
{
  XonEntity* result = nullptr;
  auto iFind = m_keyedElements.find(name);
  if (iFind != m_keyedElements.end())
  {
    result = iFind->second;
  }
  return result;
}

//==============================================================================
XonEntity& XonObject::Get(std::string const& name)
{
  auto result = TryGet(name);
  if (!result)
  {
    ThrowInvalidKeyError(name.c_str());
  }
  return *result;
}

//==============================================================================
XonEntity const& XonObject::Get(std::string const& name) const
{
  auto result = TryGet(name);
  if (!result)
  {
    ThrowInvalidKeyError(name.c_str());
  }
  return *result;
}

//==============================================================================
void XonObject::AddElement(XonEntity& value)
{
  m_elements.emplace_back(&value);
}

//==============================================================================
void XonObject::AddElement(std::string_view key, XonEntity& value)
{
  AddElement(value);
  m_keyedElements[std::string{ key }] = &value;
}

//==============================================================================
void XonObject::GetKeys(std::vector<std::string>& keys) const
{
  if (keys.capacity() < m_keyedElements.size())
  {
    keys.reserve(m_keyedElements.size());
  }

  keys.clear();
  GetKeys(std::back_inserter(keys));
}

//==============================================================================
bool XonObject::HasElement(XonEntity const& value) const
{
  return std::find_if(m_elements.begin(), m_elements.end(), [p = &value](auto& v) {
    return p == v.get();
  }) != m_elements.end();
}

//==============================================================================
XonEntity& XonObject::operator[](size_t index)
{
  if (index >= m_elements.size())
  {
    ThrowIndexOutOfBoundsError(index, m_elements.size());
  }
  return *m_elements[index];
}

//==============================================================================
XonEntity const& XonObject::operator[](size_t index) const
{
  if (index >= m_elements.size())
  {
    ThrowIndexOutOfBoundsError(index, m_elements.size());
  }
  return *m_elements[index];
}

//==============================================================================
XonValue::XonValue() = default;

//==============================================================================
XonValue::XonValue(const char* value, size_t length):
  m_value{ nullptr },
  m_length{ length }
{
  if (value)
  {
    m_value = new char[length + 1];
    std::copy(value, value + length, m_value);
    m_value[length] = '\0';
  }
}

//==============================================================================
XonValue::XonValue(std::string const& string):
  XonValue{ string.c_str(), string.size() }
{}

//==============================================================================
XonValue::~XonValue()
{
  delete[] m_value;
}

//==============================================================================
XonEntity::Type XonValue::GetType() const
{
  return Type::Value;
}

} // xr
