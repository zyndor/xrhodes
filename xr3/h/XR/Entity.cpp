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
#include "Named.hpp"

namespace XR {

//==============================================================================
Entity::Component::Component()
: m_pOwner(0)
{}

//==============================================================================
Entity::Component::~Component()
{}

//==============================================================================
void  Entity::Component::SetOwner(Entity* p)
{
  if(m_pOwner != p)
  {
    if(m_pOwner != 0)
    {
      m_pOwner->RemoveComponent(this);
    }
    
    if(p != 0)
    {
      p->AddComponent(this);
    }
  }
}

//==============================================================================
Entity::Entity()
: Named(),
  translation(),
  rotation(),
  scaling(),
  m_pParent(0),
  m_xForm(),
  m_children(),
  m_components()
{}

//==============================================================================
Entity::Entity(uint32 name)
: Named(name),
  translation(),
  rotation(),
  scaling(),
  m_pParent(0),
  m_xForm(),
  m_children(),
  m_components()
{}

//==============================================================================
Entity::Entity(const char* pName)
: Named(pName),
  translation(),
  rotation(),
  scaling(),
  m_pParent(0),
  m_xForm(),
  m_children(),
  m_components()
{}

//==============================================================================
Entity::~Entity()
{
  DetachFromParent();
  std::for_each(m_children.begin(), m_children.end(), Deleter<Entity>);
  std::for_each(m_components.begin(), m_components.end(), Deleter<Component>);
}

//==============================================================================
Matrix  Entity::GetWorldTransform() const
{
  Matrix  m(m_xForm);
  if(m_pParent != 0)
  {
    m *= m_pParent->GetWorldTransform();
  }
  return m;
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
void  Entity::DetachFromParent()
{
  if(m_pParent != 0)
  {
    m_pParent->RemoveChild(this);
    m_pParent = 0;
  }
}

//==============================================================================
void  Entity::AddChild(Entity* p)
{
  XR_ASSERTMSG(Entity, p != 0, ("Null child is invalid."));
#if defined XR_DEBUG && !defined XR_DEBUG_PERFORMANCE
  // check for cyclic dependency - p being a parent of this somewhere higher up.
  Entity* pParent(this);
  do
  {
    XR_ASSERTMSG(Entity, pParent != p,
      ("Adding child creates cyclic dependency"));
    pParent = pParent->GetParent();
  }
  while(pParent != 0);
  
  // check for redundancy or name clash.
  for(List::const_iterator i0(m_children.begin()), i1(m_children.end());
    i0 != i1; ++i0)
  {
    const Entity* p1(*i0);
    XR_ASSERTMSG(Entity, p1 != p, ("Child already added"));
    XR_ASSERTMSG(Entity, p1->GetName() != p->GetName(),
      ("Child with same name already exists."));
  }
#endif  //XR_DEBUG

  m_children.push_back(p);
}

//==============================================================================
void  Entity::RemoveChild(Entity* p)
{
  m_children.remove(p);
}

//==============================================================================
#if defined XR_DEBUG && !defined XR_DEBUG_PERFORMANCE
struct  FindEntry
{
  uint32      hash;
  const char* pPath;
};
#endif  //XR_DEBUG

Entity* Entity::FindChild(const char* pName) const
{
  XR_ASSERT(Entity, pName != 0);
#if defined XR_DEBUG && !defined XR_DEBUG_PERFORMANCE
  std::list<FindEntry> hierarchy;
#else
  std::list<uint32> hierarchy;
#endif  //XR_DEBUG

  const char* pBegin(pName);
  const char* pEnd(pName + strlen(pName));
  do
  {
    const char* pNext(strchr(pName, SEPARATOR));
    if(pNext == 0)
    {
      pNext = pEnd;
    }
    else
    {
      ++pNext;
    }

    int len(pNext - pBegin);
    XR_ASSERT(Entity, len > 0);
    uint32  hash(TransformName(pBegin, len));
#if defined XR_DEBUG && !defined XR_DEBUG_PERFORMANCE
    FindEntry  r = { hash, pName };
    hierarchy.push_back(r);
#else
    hierarchy.push_back(hash);
#endif  //XR_DEBUG

    pBegin = pNext;
  }
  while(pBegin != pEnd);

  Entity* pFound(hierarchy.empty() ? 0 : const_cast<Entity*>(this));
#if defined XR_DEBUG && !defined XR_DEBUG_PERFORMANCE
  for(std::list<FindEntry>::iterator i0(hierarchy.begin()), i1(hierarchy.end());
    pFound != 0 && i0 != i1; ++i0)
  {
    pFound = pFound->FindChild(i0->hash);
    XR_ASSERTMSG(Entity, pFound != 0, ("FindChild failed at %s.",
      i0->pPath));
#else
  for(std::list<uint32>::iterator i0(hierarchy.begin()), i1(hierarchy.end());
    pFound != 0 && i0 != i1; ++i0)
  {
    pFound = pFound->FindChild(*i0);
#endif  //XR_DEBUG
  }

  return pFound;
}

//==============================================================================
Entity* Entity::FindChild(uint32 name) const
{
  List::const_iterator  iEnd(m_children.end());
  List::const_iterator  iFind(std::find_if(m_children.begin(), iEnd,
    FindPredicate(name)));
  return iFind != iEnd ? *iFind : 0;
}

//==============================================================================
void  Entity::AddComponent(Component* p)
{
  XR_ASSERT(Entity, p != 0);
  if(p->GetOwner() != this)
  {
#if defined XR_DEBUG && !defined XR_DEBUG_PERFORMANCE
    XR_ASSERTMSG(Entity, FindComponent(p->GetType()),
      ("Entity already has a component of type %d.", p->GetType()));
#endif  //XR_DEBUG
    
    p->SetOwner(this);
    m_components.push_back(p);
  }
}

//==============================================================================
void  Entity::RemoveComponent(Component* p)
{
  m_components.remove(p);
}

//==============================================================================

Entity::Component*  Entity::FindComponent(uint32 typeId) const
{
  Component::List::const_iterator iFind(std::find_if(m_components.begin(),
    m_components.end(), Component::FindPredicate(typeId)));
  return iFind != m_components.end() ? *iFind : 0;
}

//==============================================================================
Entity* Entity::Clone(Entity* pParent) const
{
  Entity* pEntity(new Entity(GetName()));
  pEntity->translation = translation;
  pEntity->rotation = rotation;
  pEntity->scaling = scaling;
  //pEntity->UpdateTransform(); // WATCH
 
  if(pParent != 0)
  {
    pParent->AddChild(pEntity);
  }
  
  for(Component::List::const_iterator i0(m_components.begin()), i1(m_components.end());
    i0 != i1; ++i0)
  {
    const Component* pComp(*i0);
    pEntity->m_components.push_back(pComp->Clone());
  }
  
  for(List::const_iterator i0(m_children.begin()), i1(m_children.end());
    i0 != i1; ++i0)
  {
    const Entity* pChild(*i0);
    pChild->Clone(pEntity);
  }

  return pEntity;
}

} // XR
