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
#include <set>
#include "vectypes.hpp"
#include "Name.hpp"
#include "typeutils.hpp"

namespace XR
{

//==============================================================================
///@brief Entity has a 3D transformation composed of translation, rotation and
/// scaling (applied in this order), a list of Components and a list of child
/// Entities (both of which it has ownership of).
class Entity
{
  XR_NONCOPY_DECL(Entity)
  
public:
  // types
  typedef std::list<Entity*>  List;

  class FindPredicate
  {
  public:
    // structor
    explicit FindPredicate(Name n)
    : m_name(n)
    {}

    // operators
    bool  operator()(const Entity* p) const
    {
      return p->GetName() == m_name;
    }

  private:
    // data
    Name  m_name;
  };
  
  ///@brief Component class which define a single unique aspect of an Entity,
  /// with access to the Entity that owns it.
  ///@note  GetType() is expected to return the TypeId for your component class.
  /// The XR_ENTITY_COMPONENT_GETTYPE_DECL() macro helps with this.
  class Component
  {
    XR_NONCOPY_DECL(Component)
  
  public:
    // types
    typedef std::set<Component*> Set;
    
    ///@brief Finds a component that matches the typedId it's constructed with.
    class FindPredicate
    {
    public:
      // structor
      explicit FindPredicate(size_t typeId)
      : m_typeId(typeId)
      {}
      
      // operators
      bool  operator()(const Component* p) const
      {
        return p->GetTypeId() < m_typeId;
      }
      
    private:
      // data
      size_t  m_typeId;
    };
    
    // structors
    Component();
    virtual ~Component();
    
    // virtual
    virtual size_t      GetTypeId() const =0;
    virtual Component*  Clone() const =0; // Note: don't bother copying the owner.
    
    // general
    Entity* GetOwner() const;
    void    SetOwner(Entity* pOwner);
    
  private:
    // data
    Entity* m_pOwner; // no ownership
  };

  template  <typename T>
  class ComponentT: public Component
  {
  public:
    // general
    virtual size_t  GetTypeId() const
    {
      return XR::TypeId<T>();
    }
  };
  
  // static
  ///@brief The separator character used in an Entity path to FindChild(), e.g.
  /// "World.EntryPoint.Gate"
  static const char kSeparator = '.';

  // data
  Vector3         translation;
  Quaternion      rotation;
  Vector3         scaling;
  
  // structors
  Entity();
  explicit Entity(Name n);
  ~Entity();
  
  // general
  Name            GetName() const;
  void            SetName(Name n);

  void            UpdateTransform();
  const Matrix&   GetTransform() const;
  
  Matrix          GetWorldTransform() const;
  
  Entity*         GetParent() const;
  void            DetachFromParent();
  
  void            AddChild(Entity& e);  // ownership
  void            RemoveChild(Entity& e); // release ownership

  const List&     GetChildren() const;
  
  Entity*         FindChild(const char* pPath) const; // no transfer
  Entity*         FindChild(Name n) const; // no transfer

  void            AddComponent(Component& c); // ownership
  void            RemoveComponent(Component& c);  // release ownership
  
  Component*      FindComponent(size_t typeId) const;

  ///@note Uses TypeId - assumes that your Components' GetType() does too.
  template  <typename T>
  Component*      FindComponent() const;
  
  Entity*         Clone(Entity* pParent) const;

protected:
  // data
  Entity*         m_pParent;  // no ownership
  Matrix          m_xForm;
  
  List            m_children; // yes ownership
  Component::Set  m_components; // yes ownership

  Name            m_name;
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
Name  Entity::GetName() const
{
  return m_name;
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
inline
const Entity::List& Entity::GetChildren() const
{
  return m_children;
}

//==============================================================================
template  <typename T>
inline
Entity::Component*  Entity::FindComponent() const
{
  return FindComponent(ComponentT<T>::GetTypeId());
}

} // XR

#endif // XR_ENTITY_HPP
