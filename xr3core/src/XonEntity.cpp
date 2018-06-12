//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
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
XonEntity& XonEntity::operator[](size_t index)
{
  ThrowInvalidTypeError("operator[]");
  return *this; // won't.
}

//==============================================================================
XonEntity const& XonEntity::operator[](size_t index) const
{
  ThrowInvalidTypeError("operator[]");
  return *this; // won't.
}

//==============================================================================
XonEntity& XonEntity::Find(std::string const& name)
{
  ThrowInvalidTypeError("Find()");
  return *this; // won't.
}

//==============================================================================
XonEntity const& XonEntity::Find(std::string const& name) const
{
  ThrowInvalidTypeError("Find()");
  return *this; // won't.
}
//==============================================================================
const char * XonEntity::GetValue() const
{
  ThrowInvalidTypeError("GetValue()");
  return nullptr; // won't.
}

//==============================================================================
size_t XonEntity::GetNumElements() const
{
  ThrowInvalidTypeError("GetNumElements()");
  return -1; // won't.
}

//==============================================================================
XonObject::XonObject()
: XonEntity(Type::Object)
{}

//==============================================================================
XonObject::~XonObject()
{
  std::for_each(m_values.begin(), m_values.end(), [](XonEntity* p) { delete p; });
}

XonEntity& XonObject::operator[](size_t index)
{
  if (index >= m_values.size())
  {
    ThrowIndexOutOfBoundsError(index, m_values.size());
  }
  return *m_values[index];
}

//==============================================================================
XonEntity const& XonObject::operator[](size_t index) const
{
  if (index >= m_values.size())
  {
    ThrowIndexOutOfBoundsError(index, m_values.size());
  }
  return *m_values[index];
}

//==============================================================================
XonEntity& XonObject::Find(std::string const& name)
{
  auto iFind = m_keyValues.find(name);
  if (iFind == m_keyValues.end())
  {
    ThrowInvalidKeyError(name.c_str());
  }
  return *iFind->second;
}

//==============================================================================
XonEntity const& XonObject::Find(std::string const& name) const
{
  auto iFind = m_keyValues.find(name);
  if (iFind == m_keyValues.end())
  {
    ThrowInvalidKeyError(name.c_str());
  }
  return *iFind->second;
}

//==============================================================================
void XonObject::AddValue(XonEntity& value)
{
  m_values.push_back(&value);
}

//==============================================================================
void XonObject::AddValue(std::string key, XonEntity& value)
{
  AddValue(value);
  m_keyValues[key] = &value;
}

//==============================================================================
void XonObject::GetKeys(std::vector<std::string>& keys)
{
  if(keys.size() < m_keyValues.size())
  {
    keys.reserve(m_keyValues.size());
  }

  keys.clear();
  std::for_each(m_keyValues.begin(), m_keyValues.end(),
    [&keys](decltype(m_keyValues)::value_type const& v) {
      keys.push_back(v.first);
    });
}

//==============================================================================
bool XonObject::HasValue(XonEntity const& value) const
{
  return std::find(m_values.begin(), m_values.end(), &value) != m_values.end();
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
