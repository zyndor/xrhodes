//
// Nuclear Heart Games
// XRhodes
//
// GameObject.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    11/03/2014
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_ENTITY_HPP
#define XR_ENTITY_HPP

#include <list>
#include "vectypes.hpp"
#include "Named.hpp"
#include "typeutils.hpp"

namespace XR
{

//==============================================================================
///@brief Entity has a 3D transformation composed of translation, rotation and
/// scaling (in this order), a list of Components and a list of child Entities
/// (both of which it has ownership of).
class Entity: public Named
{
  XR_NONCOPY_DECL(Entity)
  
public:
  // types
  ///@brief Component class which define a single unique aspect of an Entity,
  /// with access to the Entity that owns it.
  ///@note  GetType() is expected to return the TypeId for your component class.
  /// The XR_ENTITY_COMPONENT_GETTYPE_DECL() macro helps with this.
  class Component
  {
  public:
    // types
    typedef std::list<Component*> List;
    
    ///@brief Finds a component that matches the typedId it's constructed with.
    class FindPredicate
    {
    public:
      // structor
      explicit FindPredicate(uint32 typeId)
      : m_typeId(typeId)
      {}
      
      // operators
      bool  operator()(const Component* p) const
      {
        return p->GetTypeId() == m_typeId;
      }
      
    private:
      // data
      uint32  m_typeId;
    };
    
    // structors
    Component();
    virtual ~Component();
    
    // virtual
    virtual uint32      GetTypeId() const =0;
    virtual Component*  Clone() const =0;
    
    // general
    Entity* GetOwner() const;
    void    SetOwner(Entity* pOwner);
    
  protected:
    // data
    Entity* m_pOwner; // no ownership
  };
  
  ///@brief The separator character used in an Entity path to FindChild(), e.g.
  /// "World.EntryPoint.Gate"
  enum
  {
    SEPARATOR = '.'
  };

  // data
  Vector3     translation;
  Quaternion  rotation;
  Vector3     scaling;
  
  // structors
  Entity();
  explicit Entity(uint32 name);
  explicit Entity(const char* pName);
  ~Entity();
  
  // general
  void          UpdateTransform();
  const Matrix& GetTransform() const;
  
  Matrix        GetWorldTransform() const;
  
  Entity*       GetParent() const;
  void          DetachFromParent();
  
  void          AddChild(Entity* p);  // ownership
  void          RemoveChild(Entity* p); // release ownership
  
  Entity*       FindChild(const char* pName) const; // no transfer
  Entity*       FindChild(uint32 hash) const; // no transfer
  
  void          AddComponent(Component* p); // ownership
  void          RemoveComponent(Component* p);  // release ownership
  
  Component*    FindComponent(uint32 type) const;
  
  ///@note Uses TypeId - assumes that your Components' GetType() does too.
  template  <typename T>
  Component*    FindComponent() const;
  
  Entity*       Clone(Entity* pParent) const;

protected:
  // types
  typedef std::list<Entity*>    List;
  
  // data
  Entity*         m_pParent;  // no ownership
  Matrix          m_xForm;
  
  List            m_children; // yes ownership
  Component::List m_components; // yes ownership
};

//==============================================================================
// implementation
//==============================================================================
inline
Entity* Entity::Component::GetOwner() const
{
  return m_pOwner;
}

//==============================================================================
inline
const Matrix& Entity::GetTransform() const
{
  return m_xForm;
}

//==============================================================================
inline
Entity* Entity::GetParent() const
{
  return m_pParent;
}

//==============================================================================
template  <typename T>
Entity::Component*  Entity::FindComponent() const
{
  return FindComponent(TypeId<T>());
}

} // XR

//==============================================================================
#define XR_ENTITY_COMPONENT_GETTYPE_DECL(className)\
  public:\
    virtual uint32  GetType() const\
    {\
      return TypeId<className>();\
    }\


#endif // XR_ENTITY_HPP
