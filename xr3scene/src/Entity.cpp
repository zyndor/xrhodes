//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Entity.hpp"
#include "xr/Component.hpp"
#include "xr/functions/functors.hpp"
#include "xr/utils.hpp"

namespace xr
{

//==============================================================================
char const Entity::kSeparator = '.';

//==============================================================================
Entity::Entity(Entity* parent)
: Entity(Name(), parent)
{}

//==============================================================================
Entity::Entity(Name n, Entity* parent)
: m_parent(nullptr),  // not yet
  m_firstChild(nullptr),
  m_nextSibling(nullptr),
  m_nextWorldUpdate(parent ? parent->m_nextWorldUpdate : nullptr),
  m_translation(Vector3::Zero()),
  m_rotation(Quaternion::Identity()),
  m_scale(Vector3::One()),
  m_worldTransform(parent ? parent->m_worldTransform : Matrix::Identity()),
  m_name(n)
{
  if (parent != nullptr)
  {
    parent->AddChild(*this);
  }
}

//==============================================================================
Entity::~Entity()
{
  DetachFromParent();
  while (!m_children.empty())
  {
    delete m_children.back(); // will remove itself
  }

  for (auto i: m_components)
  {
    i->m_owner= nullptr;
    delete i;
  }
}

//==============================================================================
void  Entity::SetName(Name n)
{
  m_name = n;
}

//==============================================================================
void Entity::SetTranslation(Vector3 const& t)
{
  m_translation = t;
  FlagUpdateNeeded(this);
}

//==============================================================================
void Entity::SetXTranslation(float x)
{
  m_translation.x = x;
  FlagUpdateNeeded(this);
}

//==============================================================================
void Entity::SetYTranslation(float y)
{
  m_translation.y = y;
  FlagUpdateNeeded(this);
}

//==============================================================================
void Entity::SetZTranslation(float z)
{
  m_translation.z = z;
  FlagUpdateNeeded(this);
}

//==============================================================================
void Entity::SetRotation(Quaternion const& r)
{
  m_rotation = r;
  FlagUpdateNeeded(this);
}

//==============================================================================
void Entity::SetScale(Vector3 const& s)
{
  m_scale = s;
  FlagUpdateNeeded(this);
}

//==============================================================================
void Entity::SetXScale(float x)
{
  m_scale.x = x;
  FlagUpdateNeeded(this);
}

//==============================================================================
void Entity::SetYScale(float y)
{
  m_scale.y = y;
  FlagUpdateNeeded(this);
}

//==============================================================================
void Entity::SetZScale(float z)
{
  m_scale.z = z;
  FlagUpdateNeeded(this);
}

//==============================================================================
Matrix Entity::GetLocalTransform() const
{
  Matrix xform(m_rotation, m_translation);
  xform.ScaleX(m_scale.x);
  xform.ScaleY(m_scale.y);
  xform.ScaleZ(m_scale.z);
  return xform;
}

//==============================================================================
void Entity::UpdateWorldTransform()
{
  if (m_nextWorldUpdate)
  {
    if (!m_nextWorldUpdate->GetParent())  // process root separately.
    {
      m_nextWorldUpdate->m_nextWorldUpdate = nullptr;
      m_nextWorldUpdate->m_worldTransform = m_nextWorldUpdate->GetLocalTransform();
    }

    m_nextWorldUpdate->Visit([](Entity& e) {
      e.UpdateWorldTransformInternal();
    });
  }
}

//==============================================================================
Matrix const&  Entity::GetWorldTransform(bool update) const
{
  if (update)
  {
    const_cast<Entity*>(this)->UpdateWorldTransform();
  }
  return m_worldTransform;
}

//==============================================================================
void  Entity::DetachFromParent()
{
  if (m_parent != nullptr)
  {
    m_parent->RemoveChild(*this);
  }
}

//==============================================================================
void  Entity::AddChild(Entity& e)
{
#if defined XR_DEBUG
  XR_ASSERTMSG(Entity, !e.IsAncestor(this),
    ("'%s' already a parent of '%s'; can't add as child.", e.GetName().GetDebugValue(),
      m_name.GetDebugValue()));
#endif  //XR_DEBUG

  // Check for redundancy or name clash.
  auto iInsert = FindChildIterator(e.m_name);
  if (iInsert != m_children.end() && (*iInsert)->m_name == e.m_name)
  {
    if (*iInsert == &e)
    {
      XR_TRACE(Entity, ("'%s' is already a child of '%s'.", e.m_name.GetDebugValue(),
        m_name.GetDebugValue()));
      return;
    }
    else
    {
      XR_TRACE(Entity, ("'%s' is not a unique child name on '%s'.",
        e.m_name.GetDebugValue(), m_name.GetDebugValue()));
    }
  }

  e.DetachFromParent();

  // Insert into the chain of children.
  if (m_firstChild)
  {
    auto c = m_firstChild;
    while (c->m_nextSibling)
    {
      c = c->m_nextSibling;
    }
    c->m_nextSibling = &e;
  }
  else
  {
    m_firstChild = &e;
  }

  e.m_parent = this;
  m_children.insert(iInsert, &e);

  e.FlagUpdateNeeded(m_nextWorldUpdate ? m_nextWorldUpdate : &e);
}

//==============================================================================
void Entity::AddChild(Entity& e, size_t index)
{
#if defined XR_DEBUG
  XR_ASSERTMSG(Entity, !e.IsAncestor(this),
    ("'%s' already a parent of '%s'; can't add as child.", e.GetName().GetDebugValue(),
      m_name.GetDebugValue()));
#endif  //XR_DEBUG

  // Check for redundancy or name clash.
  auto iInsert = FindChildIterator(e.m_name);
  if (iInsert != m_children.end() && (*iInsert)->m_name == e.m_name)
  {
    if (*iInsert == &e)
    {
      XR_TRACE(Entity, ("'%s' is already a child of '%s'.", e.m_name.GetDebugValue(),
        m_name.GetDebugValue()));
      return;
    }
    else
    {
      XR_TRACE(Entity, ("'%s' is not a unique child name on '%s'.",
        e.m_name.GetDebugValue(), m_name.GetDebugValue()));
    }
  }

  e.DetachFromParent();

  // Insert into the chain of children.
  Entity** iChainInsert = &m_firstChild;
  for (size_t i = 0; i < index && *iInsert; ++i)
  {
    iChainInsert = &(*iChainInsert)->m_nextSibling;
  }

  Entity* next = (*iChainInsert)->m_nextSibling;
  *iChainInsert = &e;
  e.m_nextSibling = next;

  e.m_parent = this;
  m_children.insert(iInsert, &e);

  e.FlagUpdateNeeded(m_nextWorldUpdate ? m_nextWorldUpdate : &e);
}

//==============================================================================
bool  Entity::RemoveChild(Entity& e)
{
  List::iterator  iFind = std::remove(m_children.begin(), m_children.end(), &e);
  bool result = iFind != m_children.end();
  if(result)
  {
    m_children.erase(iFind, m_children.end());

    // reorder children
    if (m_firstChild == &e)
    {
      m_firstChild = e.m_nextSibling;
    }
    else
    {
      auto c = m_firstChild;
      while (c->m_nextSibling != &e)
      {
        c = c->m_nextSibling;
      }
      c->m_nextSibling = e.m_nextSibling;
    }

    e.m_nextSibling = nullptr;
    e.m_parent = nullptr;
    e.FlagUpdateNeeded(&e);
  }
  return result;
}

//==============================================================================
Entity* Entity::FindChild(const char* path) const
{
  XR_ASSERT(Entity, path);

  const char* i0 = path;
  const char* const i1 = path + strlen(path);
  const Entity* parent = this;
  Entity* found = nullptr;
  while(i0 != i1)
  {
    const char* next = std::find(i0, i1, kSeparator);

    const ptrdiff_t len = next - i0;
    XR_ASSERT(Entity, len > 0);

    found = parent->FindChild(Name(i0, size_t(len)));
    if (!found)
    {
      XR_TRACEIF(Entity, found != nullptr, ("FindChild() failed at %s.", i0));
      break;
    }
    parent = found;

    if (next != i1)
    {
      ++next;  // skip separator
    }
    i0 = next;
  }

  return found;
}

//==============================================================================
Entity* Entity::FindChild(Name n) const
{
  List::const_iterator  iEnd = m_children.end();
  List::const_iterator  iFind = const_cast<Entity*>(this)->FindChildIterator(n);
  return (iFind != iEnd && (*iFind)->m_name == n) ? *iFind : nullptr;
}

//==============================================================================
bool Entity::AddComponent(Component& component)
{
  const size_t typeId = component.GetTypeId();
  auto iFind = FindComponentIterator(typeId);
  const bool result = iFind == m_components.end() || *iFind != &component;
  if(result)
  {
    if(component.m_owner != nullptr)
    {
      component.m_owner->RemoveComponent(component);
    }

    m_components.insert(iFind, &component);
    component.m_owner = this;
  }
  return result;
}

//==============================================================================
bool Entity::RemoveComponent(Component& component)
{
  auto iRemove = FindComponentIterator(component.GetTypeId());
  bool success = iRemove != m_components.end() && *iRemove == &component;
  if (success)
  {
    component.m_owner = nullptr;  // friend
    m_components.erase(iRemove, m_components.end());
  }
  return success;
}

//==============================================================================
Entity* Entity::Clone(Entity* parent) const
{
  // NOTE: only assign to parent following traversal.
  Entity* clone = new Entity(GetName(), nullptr);

  // copy local transformation
  clone->m_translation = m_translation;
  clone->m_rotation = m_rotation;
  clone->m_scale = m_scale;
  // NOTE: transform is updated outside, recursively.

  // clone components
  clone->m_components.reserve(m_components.capacity());
  for (auto i : m_components)
  {
    auto comp = i;
    comp = comp->Clone();
    comp->m_owner = clone;
    clone->m_components.push_back(comp);
  }

  // clone children to newly cloned entity - recursive
  for (auto i : m_children)
  {
    i->Clone(clone);
  }

  parent->AddChild(*clone);

  return clone;
}

//==============================================================================
bool Entity::IsAncestor(Entity* e) const
{
  while (e)
  {
    if (e == this)
    {
      return true;
    }
    e = e->GetParent();
  }
  return false;
}

//==============================================================================
void Entity::FlagUpdateNeeded(Entity* worldUpdate)
{
  if (worldUpdate)
  {
    if (!m_nextWorldUpdate || worldUpdate->IsAncestor(m_nextWorldUpdate))
    {
      m_nextWorldUpdate = worldUpdate;
    }

    for (auto i : m_children)
    {
      i->FlagUpdateNeeded(worldUpdate);
    }
  }
}

//==============================================================================
void Entity::UpdateWorldTransformInternal()
{
  if (m_nextWorldUpdate)
  {
    Entity* parent = GetParent();
    XR_ASSERT(Entity, parent);  // NOTE: the root should just assign local to world and unset m_nextWorldUpdate.
    XR_ASSERT(Entity, !parent->m_nextWorldUpdate);  // should already be updated.
    m_worldTransform = GetLocalTransform() * parent->m_worldTransform;
    m_nextWorldUpdate = nullptr;
  }
}

//==============================================================================
Entity::List::iterator Entity::FindChildIterator(Name name)
{
  return std::lower_bound(m_children.begin(), m_children.end(), name,
    [](Entity const* e, Name n) {
      return e->m_name < n;
    });
}

//==============================================================================
Component* Entity::FindComponent(size_t typeId)
{
  auto iFind = FindComponentIterator(typeId);
  return (iFind != m_components.end() && (*iFind)->GetTypeId() == typeId) ? *iFind : nullptr;
}

//==============================================================================
Component const* Entity::FindComponent(size_t typeId) const
{
  auto iFind = FindComponentIterator(typeId);
  return (iFind != m_components.end() && (*iFind)->GetTypeId() == typeId) ? *iFind : nullptr;
}

//==============================================================================
Entity::Components::iterator  Entity::FindComponentIterator(size_t typeId)
{
  return std::lower_bound(m_components.begin(), m_components.end(), typeId,
    [](Component* c, size_t typeId) {
      return c->GetTypeId() < typeId;
    });
}

//==============================================================================
Entity::Components::const_iterator Entity::FindComponentIterator(size_t typeId) const
{
  return std::lower_bound(m_components.begin(), m_components.end(), typeId,
    [](Component* c, size_t typeId) {
      return c->GetTypeId() < typeId;
    });
}

} // XR
