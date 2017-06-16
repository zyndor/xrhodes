//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <XR/XonEntity.hpp>
#include <XR/debug.hpp>
#include <algorithm>

namespace XR
{

//==============================================================================
XonEntity::Exception::Exception(Type t, std::string const & message)
: std::exception(message.c_str()),
  type(t)
{
  XR_TRACE(Exception, (message.c_str()));
}

//==============================================================================
XonEntity::XonEntity(Type t)
: m_type(t)
{}

//==============================================================================
XonEntity::~XonEntity()
{}

//==============================================================================
void ThrowInvalidTypeError(char const* pOperation)
{
  char arBuffer[128];
  sprintf(arBuffer, "Invalid type: %s not supported.", pOperation);
  throw XonEntity::Exception(XonEntity::Exception::Type::InvalidType, arBuffer);
}

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

//==============================================================================
void ThrowIndexOutOfBoundsError(size_t index, size_t bounds)
{
  char arBuffer[256];
  sprintf(arBuffer, "Index (%zd) out of bounds (%zd).", index, bounds);
  throw XonEntity::Exception(XonEntity::Exception::Type::IndexOutOfBounds, arBuffer);
}

XonEntity& XonObject::operator[](size_t index)
{
  if (index < m_values.size())
  {
    return *m_values[index];
  }
  else
  {
    ThrowIndexOutOfBoundsError(index, m_values.size());
  }
}

//==============================================================================
XonEntity const& XonObject::operator[](size_t index) const
{
  if (index < m_values.size())
  {
    return *m_values[index];
  }
  else
  {
    ThrowIndexOutOfBoundsError(index, m_values.size());
  }
}

//==============================================================================
void ThrowInvalidKeyError(const char* pName)
{
  char arBuffer[128];
  int len = strlen(pName);
  const int kMaxSize = 100;
  bool overSized = len > kMaxSize;
  sprintf(arBuffer, "Invalid key '%s%.*s'.", overSized ? "..." : "",
    overSized ? kMaxSize : len, pName + std::max(len - kMaxSize, 0));
  throw XonEntity::Exception(XonEntity::Exception::Type::InvalidKey, arBuffer);
}

XonEntity& XonObject::Find(std::string const& name)
{
  auto iFind = m_keyValues.find(name);
  if (iFind != m_keyValues.end())
  {
    return *iFind->second;
  }
  else
  {
    ThrowInvalidKeyError(name.c_str());
  }
}

//==============================================================================
XonEntity const& XonObject::Find(std::string const& name) const
{
  auto iFind = m_keyValues.find(name);
  if (iFind != m_keyValues.end())
  {
    return *iFind->second;
  }
  else
  {
    ThrowInvalidKeyError(name.c_str());
  }
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
    keys.resize(m_keyValues.size());
  }

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
XonValue::XonValue(const char * pValue, size_t length)
: XonEntity(Type::Value),
  m_value(pValue != nullptr ? new char[length + 1] : nullptr)
{
  if (pValue)
  {
    memcpy(m_value, pValue, length);
    m_value[length] = '\0';
  }
}

//==============================================================================
XonValue::~XonValue()
{
  delete[] m_value;
}

} // XR