//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "Deflator.hpp"

namespace XR
{

//==============================================================================
void Deflator::SetNext(IdType id)
{
  m_generator.SetNextId(id);
}

//==============================================================================
Deflator::IdType Deflator::RegisterObject(Serializable const& s)
{
  auto iFind = m_objects.find(&s);
  IdType id;
  if (iFind == m_objects.end())
  {
    id = m_generator.Generate();
    m_objects[&s] = id;
  }
  else
  {
    XR_TRACE(Deflator, ("Multiple attempts have been made to register object at %p", &s));
    id = iFind->second;
  }
  return id;
}

//==============================================================================
Deflator::IdType Deflator::GetId(Serializable const* p) const
{
  auto iFind = m_objects.find(p);
  return iFind != m_objects.end() ? iFind->second : IdGenerator::kInvalidId;
}

}
