#ifndef XR_ENTITY_HPP
#define XR_ENTITY_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Name.hpp"
#include "xr/math/Matrix.hpp"
#include "xr/math/Quaternion.hpp"
#include "xr/types/fundamentals.hpp"
#include "xr/types/typeutils.hpp"
#include <vector>

namespace xr
{

class Component;

//==============================================================================
///@brief Entity is an object in the world, with a transform and a set of
/// child Entities and Components, the latter of which are used to extend its
/// functionality (as opposed to inheritance), and the both of which they have
/// a @e notion of ownership of.
/// The children may be traversed in insertion order.
class Entity
{
  XR_NONCOPY_DECL(Entity)

public:
  // types
  typedef std::vector<Entity*>  List;

  // static
  ///@brief The separator character used in an Entity path to FindChild(), e.g.
  /// "Zone.Locators.PlayerStart"
  static const char kSeparator = '.';

  // structors
  explicit Entity(Entity* pParent);
  explicit Entity(Name n, Entity* pParent);
  ~Entity();

  // general
  Name  GetName() const;
  void  SetName(Name n);

  ///@return Local translation of this Entity.
  Vector3 const& GetTranslation() const;

  ///@brief Sets the local translation of this Entity, and flags it for the updating
  /// of local and world transforms, as well as all children for the updating of
  /// world transform.
  void SetTranslation(Vector3 const& t);

  ///@return Local rotation of this Entity.
  Quaternion const& GetRotation() const;

  ///@brief Sets the local rotation of this Entity, and flags it for the updating
  /// of local and world transforms, as well as all children for the updating of
  /// world transform.
  void SetRotation(Quaternion const& r);

  ///@return Local scale of this Entity.
  Vector3 const& GetScale() const;

  ///@brief Sets the local scale of this Entity, and flags it for the updating
  /// of local and world transforms, as well as all children for the updating of
  /// world transform.
  void SetScale(Vector3 const& s);

  ///@return Local transformation calculated from translation, rotation and scale.
  Matrix GetLocalTransform() const;

  ///@brief Updates the world transform matrix of this Entity from the local transform
  /// and the transform of the parents.
  void  UpdateWorldTransform();

  ///@brief Current (cached) world transform of this Entity.
  Matrix const&  GetWorldTransform(bool update = true) const;

  ///@return The parent of this Entity, or nullptr if none.
  ///@note Does not transfer ownership.
  Entity* GetParent() const;

  ///@return The first child of this Entity, if any (nullptr otherwise).
  ///@note Does not transfer ownership.
  Entity* GetFirstChild() const;

  ///@return The next sibling of this Entity, if any (nullptr otherwise).
  ///@note Does not transfer ownership.
  ///@note This allows to traverse the siblings in the order of insertion to the
  /// parent.
  Entity* GetNextSibling() const;

  ///@brief Removes this entity from the list of its parent's children.
  ///@note If this Entity doesn't have a parent, this is a no-op.
  void  DetachFromParent();

  ///@brief Adds the Entity @a e to this as a child, and transfers its ownership
  /// to this.
  void  AddChild(Entity& e);

  ///@brief Adds the Entity @a e to this as a child at the given @a index, and
  /// transfers its ownership to this.
  ///@note @a index is clamped to a valid value.
  void  AddChild(Entity& e, size_t index);

  ///@brief If only @a e is a child of this, relinquishes ownership of @a e and
  /// removes it from the list of children of this.
  ///@return The success of the operation.
  bool  RemoveChild(Entity& e);

  ///@brief Returns a read only list of this' children. Does not transfer
  /// ownership of any of them.
  const List&  GetChildren() const;

  ///@brief Attempts to find a child in the given @a path, which should be
  /// a kSeparator delimited list of names, e.g. "Zone.Locators.PlayerStart".
  ///@return Pointer to the child entity, or if if wasn't found, null.
  ///@note Does not transfer ownership.
  Entity*  FindChild(char const* path) const;

  ///@brief Attempts to find an immediate child with a name @a n.
  ///@return Pointer to the child entity, or if if wasn't found, null.
  ///@note Does not transfer ownership.
  Entity*  FindChild(Name n) const;

  ///@brief If this Entity has a component of type T, a pointer to it is
  /// returned, otherwise nullptr.
  template <class T>
  T*  FindComponent() const;

  ///@brief Adds a default constructed component of type T to this Entity.
  ///@return Pointer to the component, or nullptr if component of the given
  /// type has already existed on this Entity.
  ///@note Allocates the component instance.
  template <class T>
  T*  AddComponent();

  ///@brief Attempts to add the given Component instance. This will only be
  /// successful if this Entity does not yet have a component of the same type.
  /// If @A component has a previous owner, it will be removed from it.
  ///@note Transfers ownership, but only if successful.
  bool  AddComponent(Component& component);

  ///@brief If this Entity has a component of type T, removes it and deallocates
  /// the instance that was found.
  template <class T>
  void  RemoveComponent();

  ///@brief Attempts to remove the given component instance.
  ///@return The success of the operation.
  ///@note Transfers ownership, but only if successful.
  bool  RemoveComponent(Component& component);

  ///@return The success of the operation.
  ///@brief Creates a deep copy of this Entity and all of its components and
  /// child entities, then if @a parent is not nullptr, adds the clone to it
  /// as a child.
  Entity*  Clone(Entity* pParent) const;

  ///@brief Processes this Entity and all of its children using the visitor
  /// @a v, depth-first.
  ///@note Avoid operations that are either 1, alter the hierarchy, or 2,
  /// themselves recursive.
  template <class Visitor>
  void  Visit(Visitor v);

  ///@brief Processes this Entity and all of its children using the visitor
  /// @a v, depth-first.
  ///@note Avoid operations that are either 1, alter the hierarchy, or 2,
  /// themselves recursive.
  template <class Visitor>
  void  Visit(Visitor v) const;

private:
  // types
  typedef std::vector<Component*> Components;

  // data
  Entity* m_parent;  // no ownership
  Entity* m_firstChild;  // no ownership
  Entity* m_nextSibling;  // no ownership
  List  m_children; // yes ownership

  Entity* m_nextWorldUpdate;  // no ownership
  Vector3 m_translation;
  Quaternion m_rotation;
  Vector3 m_scale;
  Matrix  m_worldTransform;

  Components  m_components; // yes ownership
  Name  m_name;

  // internal
  bool IsAncestor(Entity* e) const;
  void FlagUpdateNeeded(Entity* worldUpdate);
  void UpdateWorldTransformInternal();

  List::iterator FindChildIterator(Name name);
  Component* FindComponent(size_t typeId);
  Components::iterator  FindComponentIterator(size_t typeId);
};

//==============================================================================
// implementation
//==============================================================================
inline
Name Entity::GetName() const
{
  return m_name;
}

//==============================================================================
inline
Vector3 const& Entity::GetTranslation() const
{
  return m_translation;
}

//==============================================================================
inline
Quaternion const& Entity::GetRotation() const
{
  return m_rotation;
}

//==============================================================================
inline
Vector3 const& Entity::GetScale() const
{
  return m_scale;
}

//==============================================================================
inline
Entity* Entity::GetParent() const
{
  return m_parent;
}

//==============================================================================
inline
Entity* Entity::GetFirstChild() const
{
  return m_firstChild;
}

//==============================================================================
inline
Entity* Entity::GetNextSibling() const
{
  return m_nextSibling;
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
  T* component = nullptr;
  const size_t typeId = Component::GetTypeIdImpl<T>();
  if(FindComponentIterator(typeId) == nullptr)
  {
    component = new T();
    AddComponentImpl(*component, &typeId);
  }
  return component;
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
  for (auto i : m_children)
  {
    i->Visit(v);
  }
}

//==============================================================================
template  <class Visitor>
inline
void  Entity::Visit(Visitor v) const
{
  v(*this);
  for (auto i : m_children)
  {
    i->Visit(v);
  }
}

}

#endif // XR_ENTITY_HPP
