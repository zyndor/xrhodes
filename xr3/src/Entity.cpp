//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <XR/Entity.hpp>
#include <XR/functors.hpp>

namespace XR {

//==============================================================================
Entity::Component::Component()
: m_pOwner(nullptr)
{}

//==============================================================================
Entity::Component::~Component()
{}

//==============================================================================
Entity::Entity(Entity* pParent)
: Entity(Name(), pParent)
{}

//==============================================================================
Entity::Entity(Name n, Entity* pParent)
: translation(),
  rotation(Quaternion::Unit()),
  scaling(Vector3::One()),
  m_pParent(nullptr),
  m_xForm(),
  m_children(),
  m_components(),
  m_name(n)
{
  if (pParent != nullptr)
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
  if (m_pParent != nullptr)
  {
    m *= m_pParent->GetWorldTransform();
  }
  return m;
}

//==============================================================================
void  Entity::DetachFromParent()
{
  if (m_pParent != nullptr)
  {
    m_pParent->RemoveChild(*this);
    m_pParent = nullptr;
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
  while (pParent != nullptr);
  
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
    e.m_pParent = nullptr;
  }
  return result;
}

//==============================================================================
Entity* Entity::FindChild(const char* pName) const
{
  XR_ASSERT(Entity, pName);

  const char* pBegin(pName);
  const char* const pEnd(pName + strlen(pName));
  const Entity* pParent = this;
  Entity* pFound = nullptr;
  while(pBegin != pEnd)
  {
    const char* pNext(std::find(pBegin, pEnd, kSeparator));

    const ptrdiff_t len(pNext - pBegin);
    XR_ASSERT(Entity, len > 0);

    pFound = pParent->FindChild(Name(pBegin, size_t(len)));
    XR_ASSERTMSG(Entity, pFound != nullptr, ("FindChild() failed at %s.", pBegin));
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
  return iFind != iEnd ? *iFind : nullptr;
}

//==============================================================================
Entity::Component*  Entity::FindComponent(size_t typeId) const
{
  ComponentMap::const_iterator iFind = m_components.find(typeId);
  return iFind != m_components.end() ? iFind->second : nullptr;
}

//==============================================================================
bool Entity::AddComponent(Component& component)
{
  const size_t typeId = component.GetTypeId();
  const bool result = FindComponent(typeId) == nullptr;
  if(result)
  {
    if(component.m_pOwner != nullptr)
    {
      component.m_pOwner->RemoveComponent(component);
      component.m_pOwner = nullptr;
    }

    _AddComponent(component, &typeId);
  }
  return result;
}

//==============================================================================
void Entity::_AddComponent(Component& component, const size_t* typeIdHint)
{
  XR_ASSERT(Entity, component.GetOwner() == nullptr);
  const size_t  typeId = typeIdHint != nullptr ? *typeIdHint : component.GetTypeId();
  XR_ASSERT(Entity, FindComponent(typeId) == nullptr);
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
      component.m_pOwner = nullptr;
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
