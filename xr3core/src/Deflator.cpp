//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/Deflator.hpp"
#include <sstream>

namespace xr
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
Deflator::IdType Deflator::RegisterObject(Inflatable const& obj)
{
  auto iFind = m_objects.find(&obj);
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
      m_objects[&obj] = id;
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
    XR_TRACE(Deflator, ("Object at address %p has already been registered.", &obj));
    id = iFind->second;
  }
  return id;
}

//==============================================================================
Deflator::IdType Deflator::GetId(Inflatable const* object) const
{
  auto iFind = m_objects.find(object);
  return iFind != m_objects.end() ? iFind->second : IdGenerator::kInvalidId;
}

}
