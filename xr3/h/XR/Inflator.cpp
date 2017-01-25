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
void XR::Inflator::SetNext(IdType id)
{
  m_generator.SetNextId(id);
}

//==============================================================================
Inflator::IdType Inflator::RegisterObject(Serializable & s)
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
  for (auto& id : m_mappings)
  {
    auto iFind = m_objects.find(id.first);
    if (iFind == m_objects.end())
    {
      // This means that we've deserialized an ID to which no object was registered.
      // TODO: some better exception?
      throw std::runtime_error("Failed to resolve reference.");
    }

    // Write the pointer back.
    auto pRestored = iFind->second;
    for (auto pp : id.second)
    {
      *static_cast<Serializable**>(pp) = pRestored;
    }
  }

  // Signal inflation complete.
  for (auto& obj : m_objects)
  {
    obj.second->OnInflated();
  }
}

}