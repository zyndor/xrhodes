//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "Hash.hpp"
#include "JsonEntity.hpp"
#include "functors.hpp"
#include <cstring>
#include <algorithm>
#include <iterator>
#include <sstream>

namespace XR
{
namespace JSON
{

//==============================================================================
Entity::Entity(Type t)
: m_type(t),
  m_pPrevSibling(0),
  m_pNextSibling(0)
{
  XR_ASSERT(Entity, t > ANY);
}

//==============================================================================
Entity::~Entity()
{}

//==============================================================================
Entity* Entity::LinkPrevSibling(Entity* p)
{
  Entity* pTemp(m_pPrevSibling);
  if (pTemp != 0)
  {
    pTemp->m_pNextSibling = 0;
  }
  
  m_pPrevSibling = p;
  if (p != 0)
  {
    XR_ASSERT(Entity, p->GetNextSibling() == 0);
    p->m_pNextSibling = this;
  }
  return pTemp;
}

//==============================================================================
Entity* Entity::LinkNextSibling(Entity* p)
{
  Entity* pTemp(m_pNextSibling);
  if (pTemp != 0)
  {
    pTemp->m_pPrevSibling = 0;
  }
  
  m_pNextSibling = p;
  if (p != 0)
  {
    XR_ASSERT(Entity, p->GetPrevSibling() == 0);
    p->m_pPrevSibling = this;
  }
  return pTemp;
}


//==============================================================================
Value::Value()
: Entity(VALUE),
  m_parValue(0)
{}

//==============================================================================
Value::Value(const Value& rhs)
: Entity(VALUE),
  m_parValue(0)
{
  SetValue(rhs.m_parValue);
}

//==============================================================================
Value::Value(const char* pValue, size_t len)
: Entity(VALUE),
  m_parValue()
{
  SetValue(pValue, len);
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
int Value::GetNumChildren() const
{
  return 0;
}

//==============================================================================
int Value::GetNumElements() const
{
  return 0;
}

//==============================================================================
int Value::GetValueSize() const
{
  return m_parValue != 0 ? strlen(m_parValue) : 0;
}

//==============================================================================
const char* Value::GetValue() const
{
  return m_parValue;
}

//==============================================================================
Entity* Value::GetChild(const char* pKey, Type acceptType) const
{
  return 0;
}

//==============================================================================
Entity* Value::GetElement(int id, Type acceptType) const
{
  return 0;
}

//==============================================================================
void Value::SetValue(const char* pValue)
{
  delete[] m_parValue;

  if (pValue == 0)
  {
    m_parValue = 0;
  }
  else
  {
    int len(strlen(pValue));
    
    m_parValue = new char[len + 1];
    strncpy(m_parValue, pValue, len);
    m_parValue[len] = '\0';
  }
}

//==============================================================================
void Value::SetValue(const char* pValue, int len)
{
  XR_ASSERT(Value, len >= 0);
  delete[] m_parValue;
  
  if (pValue == 0)
  {
    m_parValue = 0;
  }
  else
  {
    m_parValue = new char[len + 1];
    strncpy(m_parValue, pValue, len);
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
    i0->second.pEntity = i0->second.pEntity->Clone();
  }
}

//==============================================================================
Object::~Object()
{
  for (Child::Map::iterator i0 = m_children.begin(), i1 = m_children.end();
    i0 != i1; ++i0)
  {
    delete i0->second.pEntity;
  }
}

//==============================================================================
Object* Object::Clone() const
{
  return new Object(*this);
}

//==============================================================================
int Object::GetNumChildren() const
{
  return int(m_children.size());
}

//==============================================================================
int Object::GetNumElements() const
{
  return 0;
}

//==============================================================================
int Object::GetValueSize() const
{
  return 0;
}

//==============================================================================
const char* Object::GetValue() const
{
  return 0;
}

//==============================================================================
Entity* Object::GetChild(const char* pKey, Type acceptType) const
{
  XR_ASSERT(Object, pKey != 0);
  uint32  hash(Hash::String(pKey));
  Child::Map::const_iterator iFind(m_children.find(hash));
  Entity* pResult = 0;
  if (iFind != m_children.end())
  {
    XR_ASSERT(Object, acceptType == ANY ||
      iFind->second.pEntity->GetType() == acceptType);
    pResult = iFind->second.pEntity;
  }
  return pResult;
}

//==============================================================================
Entity* Object::GetElement(int id, Type acceptType) const
{
  return 0;
}

//==============================================================================
void  Object::AddChild(const char* pKey, Entity* pEntity)
{
  XR_ASSERT(Object, pEntity != 0);
  AddChild(std::string(pKey), pEntity);
}

//==============================================================================
void  Object::AddChild(const char* pKey, size_t keySize, Entity* pEntity)
{
  XR_ASSERT(Object, pEntity != 0);
  AddChild(std::string(pKey, keySize), pEntity);
}

//==============================================================================
void  Object::AddChild(std::string name, Entity* pEntity)
{
  XR_ASSERT(Object, pEntity != 0);
  const uint32  hash(Hash::String(name.c_str()));
  Child::Map::iterator iFind(m_children.find(hash));
  if (iFind != m_children.end())
  {
    delete iFind->second.pEntity;
    iFind->second.pEntity = pEntity;
  }
  else
  {
    Child&  c(m_children[hash]);
    c.pEntity = pEntity;
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
    XR_ASSERT(Object, hasNext || iPrev->second.pEntity->GetNextSibling() == 0);
    iPrev->second.pEntity->LinkNextSibling(pEntity);
  }
  
  if (hasNext)
  {
    XR_ASSERT(Object, hasPrev || iPrev->second.pEntity->GetPrevSibling() == 0);
    iNext->second.pEntity->LinkPrevSibling(pEntity);
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
  return i0 != m_children.end() ? i0->second.pEntity : 0;
}

//==============================================================================
Entity* Object::GetLastChild()
{
  Child::Map::reverse_iterator i0(m_children.rbegin());
  return i0 != m_children.rend() ? i0->second.pEntity : 0;
}

//==============================================================================
const Entity* Object::GetFirstChild() const
{
  Child::Map::const_iterator i0(m_children.begin());
  return i0 != m_children.end() ? i0->second.pEntity : 0;
}

//==============================================================================
const Entity* Object::GetLastChild() const
{
  Child::Map::const_reverse_iterator i0(m_children.rbegin());
  return i0 != m_children.rend() ? i0->second.pEntity : 0;
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
int Array::GetNumChildren() const
{
  return 0;
}

//==============================================================================
int Array::GetNumElements() const
{
  return m_elements.size();
}

//==============================================================================
const char* Array::GetValue() const
{
  return 0;
}

//==============================================================================
int Array::GetValueSize() const
{
  return 0;
}

//==============================================================================
void Array::AddElement(Entity* pEntity)
{
  XR_ASSERT(Object, pEntity != 0);
  Entity* pPrev(0);
  if (m_elements.size() > 0)
  {
    pPrev = *m_elements.rbegin();
  }
  pEntity->LinkPrevSibling(pPrev);
  
  m_elements.push_back(pEntity);
}

//==============================================================================
Entity* Array::GetChild(const char* pKey, Type acceptType) const
{
  return 0;
}

//==============================================================================
Entity* Array::GetElement(int id, Type acceptType) const
{
  XR_ASSERT(Array, id >= 0);
  XR_ASSERT(Array, id < static_cast<int>(m_elements.size()));
  XR_ASSERT(Object, acceptType == ANY ||
    m_elements[id]->GetType() == acceptType);
  return m_elements[id];
}

} // JSON
} // XR
