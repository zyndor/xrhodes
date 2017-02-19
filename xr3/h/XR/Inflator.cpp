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
  auto id = m_generator.Generate();
  XR_ASSERTMSG(Inflator, m_objects.find(id) == m_objects.end(),
    ("Overlapping id range detected. Id: %d.", id));
  m_objects[id] = &s;
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