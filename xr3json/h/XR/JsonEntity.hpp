//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_JSONENTITY_HPP
#define XR_JSONENTITY_HPP

#include "json.hpp"
#include <XR/debug.hpp>
#include <map>
#include <vector>
#include <list>
#include <string>
#include <stdint.h>

namespace XR
{
namespace JSON
{

//==============================================================================
class Value;
class Object;
class Array;

enum  Type
{
  ANY = -1,
  OBJECT,
  ARRAY,
  VALUE,
};

//==============================================================================
///@brief Generic JSON Entity base class.
class Entity 
{
public:
  // structors
  Entity(Type t);
  virtual ~Entity();

  // general
  Type                GetType() const;

  Value*              ToValue();
  Array*              ToArray();
  Object*             ToObject();

  Entity*             GetChild(const char* pKey) const;
  Entity*             GetElement(int id) const;

  Entity*             GetPrevSibling() const;
  Entity*             GetNextSibling() const;

  Entity*             LinkPrevSibling(Entity* p);  // returns previous value
  Entity*             LinkNextSibling(Entity* p);  // returns previous value

  // virtual
  virtual Entity*     Clone() const =0;
  
  virtual int         GetNumChildren() const =0;  // objects. arrays and values return 0
  virtual int         GetNumElements() const =0;  // arrays. objects and values return 0
  virtual int         GetValueSize() const =0;    // values. objects and arrays return 0

  virtual const char* GetValue() const =0;  // values. objects and arrays return 0

  virtual Entity*     GetChild(const char* pKey, Type acceptType) const =0;  // retrieve child from object
  virtual Entity*     GetElement(int id, Type acceptType) const =0;  // retrieve child from array
  
private:
  // data
  Type                m_type;
  
  Entity*              m_pPrevSibling; // no ownership
  Entity*              m_pNextSibling; // no ownership
};

//==============================================================================
///@brief JSON Value which is always stored as a string.
class Value:  public Entity
{
public:
  // using
  using Entity::GetChild;
  using Entity::GetElement;

  // structors
  Value();
  Value(const Value& rhs);
  explicit Value(std::string value);
  explicit Value(int i);
  explicit Value(double d);
  Value(const char* pValue, size_t len);
  ~Value();

  // general
  void                SetValue(const char* pValue);
  void                SetValue(const char* pValue, int len);
  void                SetValue(int i);
  void                SetValue(double d);
  void                SetValue(std::string str);

  virtual Value*      Clone() const;
  
  virtual int         GetNumChildren() const;  // objects. arrays and values return 0
  virtual int         GetNumElements() const;  // arrays. objects and values return 0
  virtual int         GetValueSize() const;    // values. objects and arrays return 0

  virtual const char* GetValue() const;  // values. objects and arrays return 0

  virtual Entity*     GetChild(const char* pKey, Type acceptType) const;  // retrieve child from object
  virtual Entity*     GetElement(int id, Type acceptType) const;  // retrieve child from array

protected:
  // data
  char* m_parValue;

private:
  // disabled
  Value*  ToValue();
  Array*  ToArray();
  Object* ToObject();
};

//==============================================================================
///@brief Has its own children keyed to names.
class Object: public Entity
{
public:
  // types
  typedef std::list<std::string>  StringList;
  
  // using
  using Entity::GetChild;
  using Entity::GetElement;

  // structors
  Object();
  Object(const Object& rhs);
  ~Object();

  // general
  virtual Object*     Clone() const;
  
  virtual int         GetNumChildren() const;  // objects. arrays and values return 0
  virtual int         GetNumElements() const;  // arrays. objects and values return 0
  virtual int         GetValueSize() const;    // values. objects and arrays return 0

  virtual const char* GetValue() const;  // values. objects and arrays return 0

  void                AddChild(const char* pKey, Entity* pEntity);
  void                AddChild(const char* pKey, size_t keySize, Entity* pEntity);
  void                AddChild(std::string name, Entity* pEntity);
  
  void                GetChildNames(StringList& sl) const;
  
  const Entity*       GetFirstChild() const;
  const Entity*       GetLastChild() const;
  
  Entity*             GetFirstChild();
  Entity*             GetLastChild();
  
  virtual Entity*     GetChild(const char* pKey, Type acceptType) const;  // retrieve child from object
  virtual Entity*     GetElement(int id, Type acceptType) const;  // retrieve child from array

protected:
  // types
  struct  Child
  {
    // types
    typedef std::map<uint32_t, Child>      Map;

    struct  GetName
    {
      std::string  operator()(const Map::value_type& v) const
      {
        return v.second.name;
      }
    };
    
    // data
    std::string  name;
    Entity*      pEntity;
  };
  
  // data
  Child::Map          m_children;
  
private:
  // disabled
  Value*  ToValue();
  Array*  ToArray();
  Object* ToObject();
};

//==============================================================================
///@brief Contains an array of elements. While these can be of whatever type,
/// they are not keyed to names.
class Array:  public Entity
{
public:
  // using
  using Entity::GetChild;
  using Entity::GetElement;

  // structors
  Array();
  Array(const Array& rhs);
  ~Array();

  // general
  virtual Array*      Clone() const;
  
  virtual int         GetNumChildren() const;  // objects. arrays and values return 0
  virtual int         GetNumElements() const;  // arrays. objects and values return 0
  virtual int         GetValueSize() const;    // values. objects and arrays return 0

  virtual const char* GetValue() const;  // values. objects and arrays return 0

  void                AddElement(Entity* pEntity);

  virtual Entity*     GetChild(const char* pKey, Type acceptType) const;  // retrieve child from object
  virtual Entity*     GetElement(int id, Type acceptType) const;  // retrieve child from array

protected:
  // types
  typedef std::vector<Entity*>  EntityVector;

  // data
  EntityVector  m_elements;

private:
  // disabled
  Value*  ToValue();
  Array*  ToArray();
  Object* ToObject();
};

//==============================================================================
// implementation
//==============================================================================
inline
Type Entity::GetType() const
{
  return m_type;
}

//==============================================================================
inline
Value* Entity::ToValue()
{
  XR_ASSERT(Json::Entity, m_type == VALUE);
  return m_type == VALUE ? static_cast<Value*>(this) : nullptr;
}

//==============================================================================
inline
Array* Entity::ToArray()
{
  XR_ASSERT(Json::Entity, m_type == ARRAY);
  return m_type == ARRAY ? static_cast<Array*>(this) : nullptr;
}

//==============================================================================
inline
Object* Entity::ToObject()
{
  XR_ASSERT(Json::Entity, m_type == OBJECT);
  return m_type == OBJECT ? static_cast<Object*>(this) : nullptr;
}

//==============================================================================
inline
Entity* Entity::GetChild(const char* pKey) const
{
  return GetChild(pKey, ANY);
}

//==============================================================================
inline
Entity* Entity::GetElement(int id) const
{
  return GetElement(id, ANY);
}

//==============================================================================
inline
Entity* Entity::GetPrevSibling() const
{
  return m_pPrevSibling;
}

//==============================================================================
inline
Entity* Entity::GetNextSibling() const
{
  return m_pNextSibling;
}

} // JSON
} // XR

#endif // XR_JSONENTITY_HPP
