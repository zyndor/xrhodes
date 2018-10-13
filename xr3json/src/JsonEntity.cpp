//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/JsonEntity.hpp"
#include "xr/Hash.hpp"
#include "xr/functions/functors.hpp"
#include "xr/debug.hpp"
#include <cstring>
#include <algorithm>
#include <iterator>
#include <sstream>

namespace xr
{
namespace JSON
{

//==============================================================================
Entity::Entity(Type t)
: m_type(t),
  m_pPrevSibling(nullptr),
  m_pNextSibling(nullptr)
{
  XR_ASSERT(Json::Entity, t > ANY);
}

//==============================================================================
Entity::~Entity()
{}

//==============================================================================
Entity* Entity::LinkPrevSibling(Entity* p)
{
  Entity* pTemp(m_pPrevSibling);
  if (pTemp != nullptr)
  {
    pTemp->m_pNextSibling = nullptr;
  }

  m_pPrevSibling = p;
  if (p != nullptr)
  {
    XR_ASSERT(Json::Entity, p->GetNextSibling() == nullptr);
    p->m_pNextSibling = this;
  }
  return pTemp;
}

//==============================================================================
Entity* Entity::LinkNextSibling(Entity* p)
{
  Entity* pTemp(m_pNextSibling);
  if (pTemp != nullptr)
  {
    pTemp->m_pPrevSibling = nullptr;
  }

  m_pNextSibling = p;
  if (p != nullptr)
  {
    XR_ASSERT(Json::Entity, p->GetPrevSibling() == nullptr);
    p->m_pPrevSibling = this;
  }
  return pTemp;
}


//==============================================================================
Value::Value()
: Entity(VALUE),
  m_parValue(nullptr)
{}

//==============================================================================
Value::Value(const Value& rhs)
: Entity(VALUE),
  m_parValue(nullptr)
{
  SetValue(rhs.m_parValue);
}

//==============================================================================
Value::Value(std::string value)
: Value(value.c_str(), value.size())
{}

//==============================================================================
Value::Value(const char* value, size_t len)
: Entity(VALUE),
  m_parValue()
{
  SetValue(value, len);
}

//==============================================================================
Value::Value(int i)
: Entity(VALUE),
  m_parValue()
{
  SetValue(i);
}

//==============================================================================
Value::Value(double d)
: Entity(VALUE),
  m_parValue()
{
  SetValue(d);
}

//==============================================================================
Value::~Value()
{
  delete[] m_parValue;
}

//==============================================================================
size_t Value::GetNumChildren() const
{
  return 0;
}

//==============================================================================
size_t Value::GetNumElements() const
{
  return 0;
}

//==============================================================================
size_t Value::GetValueSize() const
{
  return m_parValue != nullptr ? strlen(m_parValue) : 0;
}

//==============================================================================
const char* Value::GetValue() const
{
  return m_parValue;
}

//==============================================================================
Entity* Value::GetChild(const char* key, Type acceptType) const
{
  return nullptr;
}

//==============================================================================
Entity* Value::GetElement(int id, Type acceptType) const
{
  return nullptr;
}

//==============================================================================
void Value::SetValue(const char* value)
{
  delete[] m_parValue;

  if (value == nullptr)
  {
    m_parValue = nullptr;
  }
  else
  {
    size_t len(strlen(value));

    m_parValue = new char[len + 1];
    strncpy(m_parValue, value, len);
    m_parValue[len] = '\0';
  }
}

//==============================================================================
void Value::SetValue(const char* value, size_t len)
{
  XR_ASSERT(Json::Value, len >= 0);
  delete[] m_parValue;

  if (value == nullptr)
  {
    m_parValue = nullptr;
  }
  else
  {
    m_parValue = new char[len + 1];
    strncpy(m_parValue, value, len);
    m_parValue[len] = '\0';
  }
}

//==============================================================================
void Value::SetValue(int i)
{
  std::ostringstream  ss;
  ss << i;
  SetValue(ss.str());
}

//==============================================================================
void Value::SetValue(double d)
{
  std::ostringstream  ss;
  ss << d;
  SetValue(ss.str());
}

//==============================================================================
void  Value::SetValue(std::string str)
{
  delete[] m_parValue;

  m_parValue = new char[str.length() + 1];
  strcpy(m_parValue, str.c_str());
}

//==============================================================================
Value* Value::Clone() const
{
  return new Value(*this);
}

//==============================================================================
Object::Object()
: Entity(OBJECT),
  m_children()
{}

//==============================================================================
Object::Object(const Object& rhs)
: Entity(OBJECT),
  m_children(rhs.m_children)
{
  for (Child::Map::iterator i0 = m_children.begin(), i1 = m_children.end();
    i0 != i1; ++i0)
  {
    i0->second.entity = i0->second.entity->Clone();
  }
}

//==============================================================================
Object::~Object()
{
  for (Child::Map::iterator i0 = m_children.begin(), i1 = m_children.end();
    i0 != i1; ++i0)
  {
    delete i0->second.entity;
  }
}

//==============================================================================
Object* Object::Clone() const
{
  return new Object(*this);
}

//==============================================================================
size_t Object::GetNumChildren() const
{
  return int(m_children.size());
}

//==============================================================================
size_t Object::GetNumElements() const
{
  return 0;
}

//==============================================================================
size_t Object::GetValueSize() const
{
  return 0;
}

//==============================================================================
const char* Object::GetValue() const
{
  return nullptr;
}

//==============================================================================
Entity* Object::GetChild(const char* key, Type acceptType) const
{
  XR_ASSERT(Json::Object, key != nullptr);
  const uint64_t  hash = Hash::String(key);
  Child::Map::const_iterator iFind(m_children.find(hash));
  Entity* result = nullptr;
  if (iFind != m_children.end())
  {
    XR_ASSERT(Json::Object, acceptType == ANY ||
      iFind->second.entity->GetType() == acceptType);
    result = iFind->second.entity;
  }
  return result;
}

//==============================================================================
Entity* Object::GetElement(int id, Type acceptType) const
{
  return nullptr;
}

//==============================================================================
void  Object::AddChild(const char* key, Entity* entity)
{
  XR_ASSERT(Json::Object, entity != nullptr);
  AddChild(std::string(key), entity);
}

//==============================================================================
void  Object::AddChild(const char* key, size_t keySize, Entity* entity)
{
  XR_ASSERT(Json::Object, entity != nullptr);
  AddChild(std::string(key, keySize), entity);
}

//==============================================================================
void  Object::AddChild(std::string name, Entity* entity)
{
  XR_ASSERT(Json::Object, entity != nullptr);
  const uint64_t  hash = Hash::String(name.c_str());
  Child::Map::iterator iFind(m_children.find(hash));
  if (iFind != m_children.end())
  {
    delete iFind->second.entity;
    iFind->second.entity = entity;
  }
  else
  {
    Child& c = m_children[hash];
    c.entity = entity;
    c.name = name;
    iFind = m_children.find(hash);
  }

  Child::Map::iterator iPrev(iFind);
  Child::Map::iterator iNext(iFind);
  ++iNext;  // we know that an object keyed to hash exists.
  bool  hasPrev(iPrev != m_children.begin());
  bool  hasNext(iNext != m_children.end());
  if (hasPrev)
  {
    --iPrev;
    XR_ASSERT(Json::Object, hasNext || iPrev->second.entity->GetNextSibling() == nullptr);
    iPrev->second.entity->LinkNextSibling(entity);
  }

  if (hasNext)
  {
    XR_ASSERT(Json::Object, hasPrev || iPrev->second.entity->GetPrevSibling() == nullptr);
    iNext->second.entity->LinkPrevSibling(entity);
  }
}

//==============================================================================
void  Object::GetChildNames(StringList& sl) const
{
  std::transform(m_children.begin(), m_children.end(), std::back_inserter(sl),
    Child::GetName());
}

//==============================================================================
Entity* Object::GetFirstChild()
{
  Child::Map::iterator i0(m_children.begin());
  return i0 != m_children.end() ? i0->second.entity : nullptr;
}

//==============================================================================
Entity* Object::GetLastChild()
{
  Child::Map::reverse_iterator i0(m_children.rbegin());
  return i0 != m_children.rend() ? i0->second.entity : nullptr;
}

//==============================================================================
const Entity* Object::GetFirstChild() const
{
  Child::Map::const_iterator i0(m_children.begin());
  return i0 != m_children.end() ? i0->second.entity : nullptr;
}

//==============================================================================
const Entity* Object::GetLastChild() const
{
  Child::Map::const_reverse_iterator i0(m_children.rbegin());
  return i0 != m_children.rend() ? i0->second.entity : nullptr;
}

//==============================================================================
Array::Array()
: Entity(ARRAY),
  m_elements()
{}

//==============================================================================
Array::Array(const Array& rhs)
: Entity(ARRAY),
  m_elements(rhs.m_elements)
{
  std::transform(m_elements.begin(), m_elements.end(), m_elements.begin(),
    std::mem_fun(&Entity::Clone));
}

//==============================================================================
Array::~Array()
{
  std::for_each(m_elements.begin(), m_elements.end(), Deleter<Entity>);
}

//==============================================================================
Array*  Array::Clone() const
{
  return new Array(*this);
}

//==============================================================================
size_t Array::GetNumChildren() const
{
  return 0;
}

//==============================================================================
size_t Array::GetNumElements() const
{
  return m_elements.size();
}

//==============================================================================
const char* Array::GetValue() const
{
  return nullptr;
}

//==============================================================================
size_t Array::GetValueSize() const
{
  return 0;
}

//==============================================================================
void Array::AddElement(Entity* entity)
{
  XR_ASSERT(Json::Array, entity != nullptr);
  Entity* prev(nullptr);
  if (m_elements.size() > 0)
  {
    prev = *m_elements.rbegin();
  }
  entity->LinkPrevSibling(prev);

  m_elements.push_back(entity);
}

//==============================================================================
Entity* Array::GetChild(const char* key, Type acceptType) const
{
  return nullptr;
}

//==============================================================================
Entity* Array::GetElement(int id, Type acceptType) const
{
  XR_ASSERT(Json::Array, id >= 0);
  XR_ASSERT(Json::Array, id < static_cast<int>(m_elements.size()));
  XR_ASSERT(Json::Array, acceptType == ANY ||
    m_elements[id]->GetType() == acceptType);
  return m_elements[id];
}

} // JSON
} // xr
