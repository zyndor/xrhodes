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
Deflator::Deflator()
{}

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
    if (id == IdGenerator::kInvalidId)
    {
      throw std::logic_error("Failed to register object under reserved invalid Id.");
    }
    else if(m_ids.insert(id).second)
    {
      m_objects[&s] = id;
    }
    else
    {
      std::ostringstream str;
      str << "Id range clash detected: Id " << std::hex << id <<
        " has already been assigned to a different object.";
      throw std::logic_error(str.str());
    }
  }
  else
  {
    XR_TRACE(Deflator, ("Object at address %p has already been registered.", &s));
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
