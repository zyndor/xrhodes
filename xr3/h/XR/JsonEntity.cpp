//
// Nuclear Heart Games
// XRhodes
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#include <cstring>
#include <algorithm>
#include <sstream>
#include "Hash.hpp"
#include "JsonEntity.hpp"

namespace XR
{
namespace JSON
{

//==============================================================================  
void Entity::Deleter(Entity* p)
{
  delete p;
}

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
Entity* Entity::LinkPrevSibling( Entity* p )
{
  Entity* pTemp(m_pPrevSibling);
  m_pPrevSibling = p;
  if (p != 0)
  {
    XR_ASSERT(Entity, p->GetNextSibling() == 0);
    p->m_pNextSibling = this;
  }
  return pTemp;
}

//==============================================================================
Entity* Entity::LinkNextSibling( Entity* p )
{
  Entity* pTemp(m_pNextSibling);
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
Entity* Value::GetChild( const char* pKey, Type acceptType ) const
{
  return 0;
}

//==============================================================================
Entity* Value::GetElement( int id, Type acceptType ) const
{
  return 0;
}

//==============================================================================
void Value::SetValue(const char* pValue, int len)
{
  XR_ASSERT(Value, len >= 0);
  delete[] m_parValue;
  m_parValue = 0;
  
  if (pValue == 0)
  {
    len = 0;
  }
  else
  {
    if (len == 0)
    {
      len = strlen(pValue);
    }
    
    if (len > 0)
    {
      m_parValue = new char[len + 1];
      strncpy(m_parValue, pValue, len);
      m_parValue[len] = '\0';
    }
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
Object::Object()
: Entity(OBJECT),
  m_children()
{}

//==============================================================================
Object::~Object()
{
  while (!m_children.empty())
  {
    EntityMap::iterator iDelete(m_children.begin());
    delete iDelete->second;
    m_children.erase(iDelete);
  }
}

//==============================================================================
int Object::GetNumChildren() const
{
  return m_children.size();
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
Entity* Object::GetChild( const char* pKey, Type acceptType ) const
{
  XR_ASSERT(Object, pKey != 0);
  uint32  hash(Hash::String(pKey));
  EntityMap::const_iterator iFind(m_children.find(hash));
  if (iFind != m_children.end())
  {
    XR_ASSERT(Object, acceptType == ANY ||
      iFind->second->GetType() == acceptType);
    return iFind->second;
  }
  else
  {
    return 0;
  }
}

//==============================================================================
Entity* Object::GetElement( int id, Type acceptType ) const
{
  return 0;
}

//==============================================================================
void  Object::AddChild(const char* pKey, Entity* pEntity)
{
  XR_ASSERT(Object, pEntity != 0);
  AddChild(Hash::String(pKey), pEntity);
}

//==============================================================================
void  Object::AddChild(const char* pKey, int keySize, Entity* pEntity)
{
  XR_ASSERT(Object, pEntity != 0);
  AddChild(Hash::String(pKey, keySize), pEntity);
}

//==============================================================================
void  Object::AddChild(uint32 hash, Entity* pEntity)
{
  XR_ASSERT(Object, pEntity != 0);
  EntityMap::iterator iFind(m_children.find(hash));
  if (iFind != m_children.end())
  {
    delete iFind->second;
  }
  m_children[hash] = pEntity;

  if (m_children.size() > 1)
  {
    iFind = m_children.find(hash);
    
    EntityMap::iterator iPrev(iFind);
    EntityMap::iterator iNext(iFind);
    ++iNext;
    bool  hasPrev(iPrev != m_children.begin());
    bool  hasNext(iNext != m_children.end());
    if (hasPrev)
    {
      --iPrev;
      XR_ASSERT(Object, hasNext || iPrev->second->GetNextSibling() == 0);
      iPrev->second->LinkNextSibling(pEntity);
    }
    if (hasNext)
    {
      XR_ASSERT(Object, hasPrev || iPrev->second->GetPrevSibling() == 0);
      iNext->second->LinkPrevSibling(pEntity);      
    }
  }
}

//==============================================================================
Entity* Object::GetFirstChild()
{
  EntityMap::iterator i0(m_children.begin());
  return i0 != m_children.end() ? i0->second : 0;
}

//==============================================================================
Entity* Object::GetLastChild()
{
  EntityMap::reverse_iterator i0(m_children.rbegin());
  return i0 != m_children.rend() ? i0->second : 0;
}

//==============================================================================
const Entity* Object::GetFirstChild() const
{
  EntityMap::const_iterator i0(m_children.begin());
  return i0 != m_children.end() ? i0->second : 0;
}

//==============================================================================
const Entity* Object::GetLastChild() const
{
  EntityMap::const_reverse_iterator i0(m_children.rbegin());
  return i0 != m_children.rend() ? i0->second : 0;
}

//==============================================================================
Array::Array()
: Entity(ARRAY),
  m_elements()
{}

//==============================================================================
Array::~Array()
{
  std::for_each(m_elements.begin(), m_elements.end(), Deleter);
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
Entity* Array::GetChild( const char* pKey, Type acceptType ) const
{
  return 0;
}

//==============================================================================
Entity* Array::GetElement( int id, Type acceptType ) const
{
  XR_ASSERT(Array, id >= 0);
  XR_ASSERT(Array, id < static_cast<int>(m_elements.size()));
  XR_ASSERT(Object, acceptType == ANY ||
    m_elements[id]->GetType() == acceptType);
  return m_elements[id];
}

} // JSON
} // XR
