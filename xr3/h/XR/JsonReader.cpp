#include <cstring>
#include <algorithm>
#include <sstream>
#include "JsonReader.hpp"
#include "Hash.hpp"
#include "File.hpp"
#include "utils.hpp"

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
{}

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
      std::string str(pValue, len);
      SetValue(str);
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

//==============================================================================
Reader::Reader(int depth_)
: maxDepth(depth_),
  depth(0),
  m_state()
{}

//==============================================================================
Reader::~Reader()
{}

//==============================================================================
Entity* Reader::Read(const char* parBuffer, int size)
{
  XR_ASSERT(Reader, parBuffer != 0);
  XR_ASSERT(Reader, size >= 0);
  m_state.SetBuffer(parBuffer, size);
  depth = 0;
  
  Entity* pEntity(0);
  const char* pChar(m_state.ExpectChar()); // drop leading whitespace
  bool        result(!m_state.IsOver(pChar));
  if (result)
  {
    bool  isObject(*pChar == kObjectBegin);
    bool  isArray(*pChar == kArrayBegin);
    
    result = (isObject || isArray) && !m_state.IsOver(m_state.SkipChar());
    if (result)
    {
      if (isObject)
      {
        pEntity = new Object();
        result = (++depth < maxDepth) &&  // check exceeding depth (not an error)
          _ParseObject(pEntity->ToObject());
      }
      else
      {
        pEntity = new Array();
        result = (++depth < maxDepth) &&  // check exceeding depth (not an error)
          _ParseArray(pEntity->ToArray());
      }

      if (!result)
      {
        delete pEntity;
        pEntity = 0;
      }      
    }
  }
   
  return pEntity;
}

//==============================================================================
bool  Reader::_ParseArray(Array* pArray)
{
  const char* pChar(m_state.ExpectChar()); // look for array end or value
  bool result(!m_state.IsOver(pChar));
  if (result)
  {
    if (*pChar == kArrayEnd)
    {
      --depth;
      m_state.SkipChar();
    }
    else while (result)
    {
      Entity* pChild(_ParseValue());

      result = pChild != 0;
      if (!result)
      {
        break;
      }

      pArray->AddElement(pChild);

      pChar = m_state.ExpectChar();
      result = !m_state.IsOver(pChar) && (*pChar == kComma || *pChar == kArrayEnd);
      if (!result)
      {
        break;
      }
      
      m_state.SkipChar(); // leave character
      if (*pChar == kArrayEnd)
      {
        --depth;
        break;
      }
    }
  }
  return result;
}

//==============================================================================
bool  Reader::_ParseObject(Object* pObj)
{
  const char* pChar(m_state.ExpectChar());
  bool  result(!m_state.IsOver(pChar));
  if (result)
  {
    if (*pChar == kObjectEnd)
    {
      --depth;
      m_state.SkipChar();
    }
    else while (result)
    {
      pChar = m_state.ExpectChar();
      result = *pChar == kQuot;
      if (!result)
      {
        break;
      }

      const char* pKey(m_state.SkipChar()); // leave the quot
      result = !m_state.IsOver(pKey);
      if (!result)
      {
        break;
      }

      const char* pKeyEnd(m_state.RequireChar(kQuot)); // get other quot
      result = !m_state.IsOver(pKeyEnd);
      if (!result)
      {
        break;
      }

      result = !m_state.IsOver(m_state.SkipChar());
      if (!result)
      {
        break;
      }

      pChar = m_state.ExpectChar();  // look for colon
      result = !m_state.IsOver(pChar) && *pChar == kColon;
      if (!result)
      {
        break;
      }

      result = !m_state.IsOver(m_state.SkipChar());
      if (!result)
      {
        break;
      }

      Entity* pChild(_ParseValue());
      result = pChild != 0;
      if (!result)
      {
        break;
      }

      // add child
      pObj->AddChild(pKey, pKeyEnd - pKey, pChild);

      pChar = m_state.ExpectChar();
      result = !m_state.IsOver(pChar) && (*pChar == kComma || *pChar == kObjectEnd);
      if (!result)
      {
        break;
      }

      m_state.SkipChar();
      if (*pChar == kObjectEnd)
      {
        --depth;
        break;
      }
    }
  }
  return result;
}

//==============================================================================
Entity* Reader::_ParseValue()
{
  Entity* pChild(0);
  const char* pChar = m_state.ExpectChar();
  if (!m_state.IsOver(pChar))
  {
    if (*pChar == kQuot) // string
    {
      pChar = m_state.SkipChar();
      if (!m_state.IsOver(pChar))
      {
        const char* pValueEnd(m_state.RequireChar(kQuot)); // look for quot pair
        if (!m_state.IsOver(pValueEnd))
        {
          pChild = new Value(pChar, pValueEnd - pChar);
          m_state.SkipChar();
        }
      }
    }
    else if (isalnum(*pChar) || *pChar == '-')
    {
      int l(0);
      if (!isalpha(*pChar))
      {
        pChild = new Value(atof(pChar));
        
        l = pChild->ToValue()->GetValueSize();
      }
      else if (l = strlen(kTrue), strncmp(pChar, kTrue, l) == 0)
      {
        pChild = new Value(1);
      }
      else if (l = strlen(kFalse), strncmp(pChar, kFalse, l) == 0)
      {
        pChild = new Value(0);
      }
      else if (l = strlen(kNull), strncmp(pChar, kNull, l) == 0)
      {
        pChild = new Value();
      }
      
      if (l > 0)
      {
        int left(m_state.GetNumCharsLeft());
        if (left < l)
        {
          l = left;
        }
        
        m_state.FastForward(l);
      }
    }
    else if (*pChar == kObjectBegin)
    {
      pChild = new Object();
      if (!m_state.IsOver(m_state.SkipChar()) &&
        depth + 1 < maxDepth)
      {
        ++depth;
        if(!_ParseObject(pChild->ToObject()))
        {
          delete pChild;
          pChild = 0;
        }
      }
    }
    else if (*pChar == kArrayBegin)
    {
      pChild = new Array();
      if (!m_state.IsOver(m_state.SkipChar()) &&
        depth + 1 < maxDepth)
      {
        ++depth;
        if(!_ParseArray(pChild->ToArray()))
        {
          delete pChild;
          pChild = 0;
        }
      }
    }
  }
  return pChild;
}

//==============================================================================
Entity* LoadJSON( const char* pFilename, int maxDepth )
{
  XR_ASSERT(LoadJSON, pFilename != 0);
  int hFile(File::Open(pFilename, "rb"));
  if (hFile == File::INVALID_HANDLE)
  {
    XR_ERROR(("Failed to open file '%s'", pFilename));
    return 0;
  }
  
  int size(File::GetSize(hFile));
  char* parBuffer = new char[size];

  bool  result(File::Read(hFile, size, 1, parBuffer) != 0);
  File::Close(hFile);
  
  if (!result)
  {
    XR_ERROR(("Failed to read contents of '%s': %s", pFilename,
      File::GetErrorString()));
    delete[] parBuffer;
    return 0;
  }
  
  // parse parse parse
  Reader  reader(maxDepth);
  Entity* pJson(reader.Read(parBuffer, size));
  
  // done, clean up
  delete[] parBuffer;
  if (pJson == 0)
  {
    XR_ERROR(("Failed to parse '%s': error around row %d char %d",
      pFilename, reader.GetState().GetRow(), reader.GetState().GetColumn()));
  }
  
  return pJson;
}

} // JSON
} // XR
