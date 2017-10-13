//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_ENTITY_HPP
#define XR_ENTITY_HPP

#include "Matrix.hpp"
#include "Quaternion.hpp"
#include "Name.hpp"
#include "XR/fundamentals.hpp"
#include "XR/typeutils.hpp"
#include <list>
#include <map>

namespace XR
{

//==============================================================================
///@brief Entity has a 3D transformation composed of translation, rotation and
/// scaling (applied in this order), a set of Components and a list of child
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
  
  ///@brief Component class which defines a single unique aspect of an Entity,
  /// with access to the Entity that owns it.
  class Component
  {
    XR_NONCOPY_DECL(Component)
  
  public:
    // static
    template <typename T>
    static size_t GetTypeIdImpl()
    {
      return XR::TypeId<T>();
    }

    // structors
    Component();
    virtual ~Component();
    
    // virtual
    ///@brief Returns the type id of the component.
    virtual size_t      GetTypeId() const =0;

    ///@brief Creates a copy of the component; this instance will be returned.
    virtual Component*  Clone() const =0;

    // general
    Entity* GetOwner() const;

  private:
    // friend
    friend class Entity;

    // data
    Entity* m_pOwner; // no ownership
  };

  ///@brief Helper class to automate the definition of GetTypeId().
  template  <typename T>
  class ComponentT: public Component
  {
  public:
    // types
    typedef ComponentT<T> BaseType;

    // general
    virtual size_t  GetTypeId() const
    {
      return GetTypeIdImpl<T>();
    }
  };
  
  // static
  ///@brief The separator character used in an Entity path to FindChild(), e.g.
  /// "Zone.Locators.PlayerStart"
  static const char kSeparator = '.';

  // data
  Vector3         translation;
  Quaternion      rotation;
  Vector3         scaling;
  
  // structors
  explicit Entity(Entity* pParent);
  explicit Entity(Name n, Entity* pParent);
  ~Entity();
  
  // general
  Name            GetName() const;
  void            SetName(Name n);

  ///@brief Updates the local transform matrix of this Entity.
  void            UpdateTransform();
  const Matrix&   GetTransform() const;
  
  ///@brief Calculates and returns the world transform matrix of this Entity,
  /// based on the transforms of itself and its parents.
  Matrix          GetWorldTransform() const;
  
  ///@brief Returns the parent of this Entity, or nullptr if none.
  Entity*         GetParent() const;

  ///@brief Removes this entity from the list of its parent's children. If this
  /// Entity doesn't have a parent, this is a no-op.
  void            DetachFromParent();
  
  ///@brief Adds the Entity @a e as a child of this and transfers its ownership
  /// to this.
  void            AddChild(Entity& e);

  ///@brief If only @a e is a child of this, relinquishes ownership of it and
  /// removes it from the list of children of this.
  ///@return The success of the operation.
  bool            RemoveChild(Entity& e);

  ///@brief Returns a read only list of this' children. Does not transfer
  /// ownership of any of them.
  const List&     GetChildren() const;
  
  ///@brief Attempts to find a child in the given @a path, which should be
  /// a kSeparator delimited list of names, e.g. "Zone.Locators.PlayerStart".
  ///@return Pointer to the child entity, or if if wasn't found, null.
  ///@note Does not transfer ownership.
  Entity*         FindChild(const char* pPath) const;

  ///@brief Attempts to find an immediate child with a name @a n.
  ///@return Pointer to the child entity, or if if wasn't found, null.
  ///@note Does not transfer ownership.
  Entity*         FindChild(Name n) const;

  ///@brief If this Entity has a component of type T, a pointer to it is
  /// returned, otherwise nullptr.
  template <class T>
  T*              FindComponent() const;

  ///@brief Adds a default constructed component of type T to this Entity.
  ///@return Pointer to the component, or nullptr if component of the given
  /// type has already existed on this Entity.
  ///@note Allocates the component instance.
  template <class T>
  T*              AddComponent();

  ///@brief Attempts to add the given Component instance. This will only be
  /// successful if this Entity does not yet have a component of the same type.
  /// If @A component has a previous owner, it will be removed from it.
  ///@note Transfers ownership, but only if successful.
  bool            AddComponent(Component& component);

  ///@brief If this Entity has a component of type T, removes it and deallocates
  /// the instance that was found.
  template <class T>
  void            RemoveComponent();

  ///@brief Attempts to remove the given component instance.
  ///@return THe success of the operation.
  ///@note Transfers ownership, but only if successful.
  bool            RemoveComponent(Component& component);

  ///@return The success of the operation.
  ///@brief Creates a deep copy of this Entity and all of its components and
  /// child entities, then if @a pParent is not nullptr, adds the clone to it
  /// as a child.
  Entity*         Clone(Entity* pParent) const;

  ///@brief Processes this Entity and all of its children using the visitor
  /// @a v, depth-first.
  template <class Visitor>
  void            Visit(Visitor v);

private:
  // types
  typedef std::map<size_t, Component*> ComponentMap;

  // data
  Entity*         m_pParent;  // no ownership
  Matrix          m_xForm;
  
  List            m_children; // yes ownership
  ComponentMap    m_components; // yes ownership

  Name            m_name;

  // internal
  Component*      FindComponent(size_t typeId) const;
  void            _AddComponent(Component& component, const size_t* typeIdHint = 0);
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
Name Entity::GetName() const
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
template  <class T>
inline
T*  Entity::FindComponent() const
{
  return static_cast<T*>(FindComponent(Component::GetTypeIdImpl<T>()));
}

//==============================================================================
template  <class T>
inline
T*  Entity::AddComponent()
{
  T* pComponent = nullptr;
  const size_t typeId = Component::GetTypeIdImpl<T>();
  if(FindComponent(typeId) == nullptr)
  {
    pComponent = new T();
    _AddComponent(*pComponent, &typeId);
  }
  return pComponent;
}

//==============================================================================
template  <class T>
inline
void Entity::RemoveComponent()
{
  Component*  pComponent = FindComponent<T>();
  if(pComponent != nullptr && RemoveComponent(*pComponent))
  {
    delete pComponent;
  }
}

//==============================================================================
template  <class Visitor>
inline
void  Entity::Visit(Visitor v)
{
  v(*this);
  for(List::iterator i0 = m_children.begin(), i1 = m_children.end(); i0 != i1; ++i0)
  {
    v(**i0);
  }
}

} // XR

///@brief Declares 'name' as a derivative of XR::Entity::ComponentT<name> thereby
/// further facilitating the automation of a GetTypeId() implementation required
/// for Component subtypes.
///@usage class or struct XR_COMPONENT_DECL(MyComponent) { /* declarations */ };
///@note Can be combined with templates and multiple inheritance:
/// template <typename T> class XR_COMPONENT_DECL(MyComponent), public OtherBase{};
#define XR_COMPONENT_DECL(name) name: public XR::Entity::ComponentT<name>

#endif // XR_ENTITY_HPP
