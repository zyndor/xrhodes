//
// Nuclear Heart Interactive
// XRhodes
//
// copyright (c) 2011 - 2016. All rights reserved.
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
Entity::Entity(Entity* pParent)
: translation(),
  rotation(Quaternion::s_unit),
  scaling(Vector3::s_one),
  m_pParent(0),
  m_xForm(),
  m_children(),
  m_components(),
  m_name()
{
  if(pParent != 0)
  {
    pParent->AddChild(*this);
  }
}

//==============================================================================
Entity::Entity(Name n, Entity* pParent)
: translation(),
  rotation(Quaternion::s_unit),
  scaling(Vector3::s_one),
  m_pParent(0),
  m_xForm(),
  m_children(),
  m_components(),
  m_name(n)
{
  if (pParent != 0)
  {
    pParent->AddChild(*this);
  }
}

//==============================================================================
Entity::~Entity()
{
  DetachFromParent();
  std::for_each(m_children.begin(), m_children.end(), Deleter<Entity>);

  for(ComponentMap::iterator i0 = m_components.begin(), i1 = m_components.end();
    i0 != i1; ++i0)
  {
    delete i0->second;
  }
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
  
  e.DetachFromParent();
  m_children.push_back(&e);
  e.m_pParent = this;
}

//==============================================================================
bool  Entity::RemoveChild(Entity& e)
{
  List::iterator  iFind = std::find(m_children.begin(), m_children.end(), &e);
  bool result = iFind != m_children.end();
  if(result)
  {
    m_children.erase(iFind);
    e.m_pParent = 0;
  }
  return result;
}

//==============================================================================
Entity* Entity::FindChild(const char* pName) const
{
  XR_ASSERT(Entity, pName != 0);

  const char* pBegin(pName);
  const char* const pEnd(pName + strlen(pName));
  const Entity* pParent = this;
  Entity* pFound = 0;
  while(pBegin != pEnd)
  {
    const char* pNext(std::find(pBegin, pEnd, kSeparator));

    const ptrdiff_t len(pNext - pBegin);
    XR_ASSERT(Entity, len > 0);

    pFound = pParent->FindChild(Name(pBegin, size_t(len)));
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
Entity::Component*  Entity::FindComponent(size_t typeId) const
{
  ComponentMap::const_iterator iFind = m_components.find(typeId);
  return iFind != m_components.end() ? iFind->second : 0;
}

//==============================================================================
bool Entity::AddComponent(Component& component)
{
  const size_t typeId = component.GetTypeId();
  const bool result = FindComponent(typeId) == 0;
  if(result)
  {
    if(component.m_pOwner != 0)
    {
      component.m_pOwner->RemoveComponent(component);
      component.m_pOwner = 0;
    }

    _AddComponent(component, &typeId);
  }
  return result;
}

//==============================================================================
void Entity::_AddComponent(Component& component, const size_t* typeIdHint)
{
  XR_ASSERT(Entity, component.GetOwner() == 0);
  const size_t  typeId = typeIdHint != 0 ? *typeIdHint : component.GetTypeId();
  XR_ASSERT(Entity, FindComponent(typeId) == 0);
  m_components.insert(ComponentMap::value_type(typeId, &component));
  component.m_pOwner = this;
}

//==============================================================================
bool Entity::RemoveComponent(Component& component)
{
  bool result = false;
  ComponentMap::iterator i0 = m_components.begin();
  ComponentMap::iterator i1 = m_components.end();
  while(i0 != i1)
  {
    result = i0->second == &component;
    if(result)
    {
      component.m_pOwner = 0;
      m_components.erase(i0);
      break;
    }
    ++i0;
  }
  return result;
}

//==============================================================================
Entity* Entity::Clone(Entity* pParent) const
{
  Entity* pClone(new Entity(GetName(), pParent));
  
  // copy local transformation
  pClone->translation = translation;
  pClone->rotation = rotation;
  pClone->scaling = scaling;
  //pEntity->UpdateTransform();

  // clone components
  for (ComponentMap::const_iterator i0(m_components.begin()), i1(m_components.end());
    i0 != i1; ++i0)
  {
    const Component* pComp(i0->second);
    pClone->_AddComponent(*pComp->Clone());
  }
  
  // clone children to newly cloned entity - recursive
  for (List::const_iterator i0(m_children.begin()), i1(m_children.end());
    i0 != i1; ++i0)
  {
    const Entity* pChild(*i0);
    pChild->Clone(pClone);
  }

  return pClone;
}

} // XR
