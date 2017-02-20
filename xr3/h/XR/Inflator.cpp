//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "Inflator.hpp"

namespace XR
{

//==============================================================================
Inflator::Inflator()
{}

//==============================================================================
void XR::Inflator::SetNext(IdType id)
{
  m_generator.SetNextId(id);
}

//==============================================================================
Inflator::IdType Inflator::RegisterObject(Inflatable & s)
{
  IdType id = IdGenerator::kInvalidId;
  auto iFind = m_ids.find(&s);
  if (iFind != m_ids.end())
  {
    id = iFind->second;
  }
  else
  {
    id = m_generator.Generate();
    auto iFind2 = m_objects.find(id);
    if (iFind2 != m_objects.end())
    {
      std::ostringstream str;
      str << "Id range clash detected: Id " << std::hex << id <<
        " has already been assigned to a different object.";
      throw std::runtime_error(str.str());
    }
    m_objects[id] = &s;
    m_ids[&s] = id;
  }
  return id;
}

//==============================================================================
void Inflator::ResolveMappings()
{
  for (auto& mapping : m_mappings)
  {
    auto iFind = m_objects.find(mapping.first);
    if (iFind == m_objects.end())
    {
      // This means that we've deserialized an ID to which no object was registered.
      std::ostringstream str;
      str << "Failed to resolve " << mapping.second.size() << " pointers: ";
      str << "an object with ID " << std::hex << mapping.first << " has not been registered.";
      throw std::runtime_error(str.str());
    }

    // Write the pointer back.
    auto pRestored = iFind->second;
    for (auto pp : mapping.second)
    {
      *static_cast<Inflatable**>(pp) = pRestored;
    }
  }

  // Signal inflation complete.
  for (auto& obj : m_objects)
  {
    obj.second->OnInflated();
  }
}

}