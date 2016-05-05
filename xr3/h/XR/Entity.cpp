//
// Nuclear Heart Games
// XRhodes
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#include "Entity.hpp"
#include "functors.hpp"
#include "types.hpp"

namespace XR {

//==============================================================================
Entity::Component::Component()
: m_pOwner(0)
{}

//==============================================================================
Entity::Component::~Component()
{}

//==============================================================================
void  Entity::Component::SetOwner(Entity* pOwner)
{
#if defined XR_DEBUG && !defined XR_DEBUG_PERFORMANCE
  XR_ASSERTMSG(Entity::Component, pOwner == 0 ||
    pOwner->FindComponent(this->GetTypeId()) == this,
    ("Entity (%s) needs to add Component %p before calling SetOwner()",
      pOwner->GetName().GetDebugValue(), this));
#endif
  m_pOwner = pOwner;
}

//==============================================================================
Entity::Entity()
: translation(),
  rotation(),
  scaling(),
  m_pParent(0),
  m_xForm(),
  m_children(),
  m_components(),
  m_name()
{}

//==============================================================================
Entity::Entity(Name n)
: translation(),
  rotation(),
  scaling(),
  m_pParent(0),
  m_xForm(),
  m_children(),
  m_components(),
  m_name(n)
{}

//==============================================================================
Entity::~Entity()
{
  DetachFromParent();
  std::for_each(m_children.begin(), m_children.end(), Deleter<Entity>);
  std::for_each(m_components.begin(), m_components.end(), Deleter<Component>);
}

//==============================================================================
void  Entity::SetName(Name n)
{
  m_name = n;
}

//==============================================================================
void  Entity::UpdateTransform()
{
  m_xForm = rotation;
  m_xForm.ScaleX(scaling.x);
  m_xForm.ScaleY(scaling.y);
  m_xForm.ScaleZ(scaling.z);
  m_xForm.t = translation;
}

//==============================================================================
Matrix  Entity::GetWorldTransform() const
{
  Matrix  m(m_xForm);
  if (m_pParent != 0)
  {
    m *= m_pParent->GetWorldTransform();
  }
  return m;
}

//==============================================================================
void  Entity::DetachFromParent()
{
  if (m_pParent != 0)
  {
    m_pParent->RemoveChild(*this);
    m_pParent = 0;
  }
}

//==============================================================================
void  Entity::AddChild(Entity& e)
{
#if defined XR_DEBUG && !defined XR_DEBUG_PERFORMANCE
  // check for cyclic dependency - p being a parent of this somewhere higher up.
  Entity* pParent(this);
  do
  {
    XR_ASSERTMSG(Entity, pParent != &e,
      ("Adding child creates cyclic dependency."));
    pParent = pParent->GetParent();
  }
  while (pParent != 0);
  
  // check for redundancy or name clash.
  for (List::const_iterator i0(m_children.begin()), i1(m_children.end());
    i0 != i1; ++i0)
  {
    const Entity* p1(*i0);
    XR_ASSERTMSG(Entity, p1 != &e, ("Child already added."));
    XR_ASSERTMSG(Entity, p1->GetName() != e.GetName(),
      ("Child with same name already exists."));
  }
#endif  //XR_DEBUG

  m_children.push_back(&e);
}

//==============================================================================
void  Entity::RemoveChild(Entity& e)
{
  List::iterator  iFind = std::find(m_children.begin(), m_children.end(), &e);
  if(iFind != m_children.end())
  {
    m_children.erase(iFind);
    e.m_pParent = 0;
  }
}

//==============================================================================
Entity* Entity::FindChild(const char* pName) const
{
  XR_ASSERT(Entity, pName != 0);

  const char* pBegin(pName);
  const char* pEnd(pName + strlen(pName));
  const Entity* pParent = this;
  Entity* pFound = 0;
  while(pBegin != pEnd)
  {
    const char* pNext(std::find(pBegin, pEnd, SEPARATOR));

    const ptrdiff_t len(pNext - pBegin);
    XR_ASSERT(Entity, len > 0);

    const uint32  hash(Hash::String(pBegin, len));
    pFound = pParent->FindChild(Name(hash));
    XR_ASSERTMSG(Entity, pFound != 0, ("FindChild() failed at %s.", pBegin));
    pParent = pFound;

    if (pNext != pEnd)
    {
      ++pNext;  // skip separator
    }
    pBegin = pNext;
  }

  return pFound;
}

//==============================================================================
Entity* Entity::FindChild(Name n) const
{
  List::const_iterator  iEnd(m_children.end());
  List::const_iterator  iFind(std::find_if(m_children.begin(), iEnd,
    FindPredicate(n)));
  return iFind != iEnd ? *iFind : 0;
}

//==============================================================================
void  Entity::AddComponent(Component& c)
{
  Entity* pOwner(c.GetOwner());
  XR_ASSERT(Entity, pOwner != this);

#if defined XR_DEBUG && !defined XR_DEBUG_PERFORMANCE
  XR_ASSERTMSG(Entity, FindComponent(c.GetTypeId()) != 0,
    ("Entity already has a component of type %d; only one allowed.",
      c.GetTypeId()));
#endif  //XR_DEBUG
    
  if (pOwner != 0)
  {
    pOwner->RemoveComponent(c);
  }

  bool  inserted = m_components.insert(&c).second;
  XR_ASSERT(Entity, inserted);

  c.SetOwner(this);  // after it has actually been added
}

//==============================================================================
void  Entity::RemoveComponent(Component& c)
{
  m_components.erase(&c);
  c.SetOwner(0);
}

//==============================================================================
Entity::Component*  Entity::FindComponent(size_t typeId) const
{
  Component::Set::const_iterator iFind(std::find_if (m_components.begin(),
    m_components.end(), Component::FindPredicate(typeId)));
  return iFind != m_components.end() ? *iFind : 0;
}

//==============================================================================
Entity* Entity::Clone(Entity* pParent) const
{
  Entity* pEntity(new Entity(GetName()));
  
  // copy local transformation
  pEntity->translation = translation;
  pEntity->rotation = rotation;
  pEntity->scaling = scaling;
  //pEntity->UpdateTransform(); // WATCH
 
  if (pParent != 0)
  {
    pParent->AddChild(*pEntity);
  }
  
  // clone components
  for (Component::Set::const_iterator i0(m_components.begin()), i1(m_components.end());
    i0 != i1; ++i0)
  {
    const Component* pComp(*i0);
    pEntity->AddComponent(*pComp->Clone());
  }
  
  // clone children to newly cloned entity - recursive
  for (List::const_iterator i0(m_children.begin()), i1(m_children.end());
    i0 != i1; ++i0)
  {
    const Entity* pChild(*i0);
    pChild->Clone(pEntity);
  }

  return pEntity;
}

} // XR
