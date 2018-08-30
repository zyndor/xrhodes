//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/XonEntity.hpp"
#include <algorithm>
#include <string>

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
XonEntity::XonEntity(Type t)
: m_type(t)
{}

//==============================================================================
XonEntity::~XonEntity()
{}

//==============================================================================
bool XonEntity::Is(Type type) const
{
  return m_type == type;
}

//==============================================================================
XonObject& XonEntity::ToObject()
{
  if (m_type != Type::Object)
  {
    ThrowInvalidTypeError("ToObject");
  }
  return static_cast<XonObject&>(*this);
}

//==============================================================================
XonObject const& XonEntity::ToObject() const
{
  if (m_type != Type::Object)
  {
    ThrowInvalidTypeError("ToObject");
  }
  return static_cast<XonObject const&>(*this);
}

//==============================================================================
XonValue& XonEntity::ToValue()
{
  if (m_type != Type::Value)
  {
    ThrowInvalidTypeError("ToValue");
  }
  return static_cast<XonValue&>(*this);
}

//==============================================================================
XonValue const& XonEntity::ToValue() const
{
  if (m_type != Type::Value)
  {
    ThrowInvalidTypeError("ToValue");
  }
  return static_cast<XonValue const&>(*this);
}

//==============================================================================
XonObject::XonObject()
: XonEntity(Type::Object)
{}

//==============================================================================
XonObject::~XonObject()
{
  std::for_each(m_elements.begin(), m_elements.end(), [](XonEntity* p) { delete p; });
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
  m_elements.push_back(&value);
}

//==============================================================================
void XonObject::AddElement(std::string key, XonEntity& value)
{
  AddElement(value);
  m_keyedElements[key] = &value;
}

//==============================================================================
void XonObject::GetKeys(std::vector<std::string>& keys)
{
  if(keys.size() < m_keyedElements.size())
  {
    keys.reserve(m_keyedElements.size());
  }

  keys.clear();
  std::for_each(m_keyedElements.begin(), m_keyedElements.end(),
    [&keys](decltype(m_keyedElements)::value_type const& v) {
      keys.push_back(v.first);
    });
}

//==============================================================================
bool XonObject::HasElement(XonEntity const& value) const
{
  return std::find(m_elements.begin(), m_elements.end(), &value) != m_elements.end();
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
XonValue::XonValue()
: XonEntity(Type::Value)
{}

//==============================================================================
XonValue::XonValue(const char * value, size_t length)
: XonEntity(Type::Value),
  m_value(value != nullptr ? new char[length + 1] : nullptr)
{
  if (value)
  {
    memcpy(m_value, value, length);
    m_value[length] = '\0';
  }
}

//==============================================================================
XonValue::~XonValue()
{
  delete[] m_value;
}

} // xr
