#ifndef XR_JSONENTITY_HPP
#define XR_JSONENTITY_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "json.hpp"
#include "xr/debug.hpp"
#include <map>
#include <vector>
#include <list>
#include <string>
#include <stdint.h>

namespace xr
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

  Entity*             GetChild(const char* key) const;
  Entity*             GetElement(int id) const;

  Entity*             GetPrevSibling() const;
  Entity*             GetNextSibling() const;

  Entity*             LinkPrevSibling(Entity* p);  // returns previous value
  Entity*             LinkNextSibling(Entity* p);  // returns previous value

  // virtual
  virtual Entity*     Clone() const =0;

  virtual size_t      GetNumChildren() const =0;  // objects. arrays and values return 0
  virtual size_t      GetNumElements() const =0;  // arrays. objects and values return 0
  virtual size_t      GetValueSize() const =0;    // values. objects and arrays return 0

  virtual const char* GetValue() const =0;  // values. objects and arrays return nullptrs

  virtual Entity*     GetChild(const char* key, Type acceptType) const =0;  // retrieve child from object
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
  Value(const char* value, size_t len);
  ~Value();

  // general
  void        SetValue(const char* value);
  void        SetValue(const char* value, size_t len);
  void        SetValue(int i);
  void        SetValue(double d);
  void        SetValue(std::string str);

  Value*      Clone() const override;

  size_t      GetNumChildren() const override;  // objects. arrays and values return 0
  size_t      GetNumElements() const override;  // arrays. objects and values return 0
  size_t      GetValueSize() const override;    // values. objects and arrays return 0

  const char* GetValue() const override;  // values. objects and arrays return nullptr

  Entity*     GetChild(const char* key, Type acceptType) const override;  // retrieve child from object
  Entity*     GetElement(int id, Type acceptType) const override;  // retrieve child from array

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
  Object*     Clone() const override;

  size_t      GetNumChildren() const override;  // objects. arrays and values return 0
  size_t      GetNumElements() const override;  // arrays. objects and values return 0
  size_t      GetValueSize() const override;    // values. objects and arrays return 0

  const char* GetValue() const override;  // values. objects and arrays return nullptr

  void                AddChild(const char* key, Entity* entity);
  void                AddChild(const char* key, size_t keySize, Entity* entity);
  void                AddChild(std::string name, Entity* entity);

  void                GetChildNames(StringList& sl) const;

  const Entity*       GetFirstChild() const;
  const Entity*       GetLastChild() const;

  Entity*             GetFirstChild();
  Entity*             GetLastChild();

  Entity*     GetChild(const char* key, Type acceptType) const override;  // retrieve child from object
  Entity*     GetElement(int id, Type acceptType) const override;  // retrieve child from array

protected:
  // types
  struct  Child
  {
    // types
    typedef std::map<uint64_t, Child>      Map;

    struct  GetName
    {
      std::string  operator()(const Map::value_type& v) const
      {
        return v.second.name;
      }
    };

    // data
    std::string  name;
    Entity*      entity;
  };

  // data
  Child::Map     m_children;

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
  Array*      Clone() const override;

  size_t      GetNumChildren() const override;  // objects. arrays and values return 0
  size_t      GetNumElements() const override;  // arrays. objects and values return 0
  size_t      GetValueSize() const override;    // values. objects and arrays return 0

  const char* GetValue() const override;  // values. objects and arrays return nullptr

  void        AddElement(Entity* entity);

  Entity*     GetChild(const char* key, Type acceptType) const override;  // retrieve child from object
  Entity*     GetElement(int id, Type acceptType) const override;  // retrieve child from array

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
Entity* Entity::GetChild(const char* key) const
{
  return GetChild(key, ANY);
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
} // xr

#endif // XR_JSONENTITY_HPP
