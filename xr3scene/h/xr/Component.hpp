#ifndef XR_COMPONENT_HPP
#define XR_COMPONENT_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include <cstddef>
#include "xr/types/fundamentals.hpp"
#include "xr/types/typeutils.hpp"

namespace xr
{

class Entity;

//==============================================================================
///@brief Component class which defines a single unique aspect of an Entity,
/// with access to the Entity that owns it.
///@note Classes deriving from Component must be default constructible.
class Component
{
  XR_NONCOPY_DECL(Component)

public:
  // static
  template <typename T>
  static size_t GetTypeIdImpl()
  {
    static_assert(std::is_base_of<Component, T>(), "T must derive from Component.");
    return TypeId<T>();
  }

  // structors
  Component();
  virtual ~Component();

  // virtual
  ///@brief Returns the type id of the component.
  virtual size_t  GetTypeId() const =0;

  ///@brief Creates a copy of the component which will then be returned.
  ///@note For derived classes, the m_owner of the clone should be (left as)
  /// nullptr.
  [[nodiscard]] virtual Component*  Clone() const =0;

  // general
  ///@return The Entity that owns the component.
  Entity* GetOwner() const;

private:
  // friend
  friend class Entity;

  // data
  Entity* m_owner; // no ownership
};

///@brief Helper class to automate the definition of GetTypeId().
template  <typename T>
class ComponentT: public Component
{
public:
  // types
  typedef ComponentT<T> BaseType;

  // general
  virtual size_t  GetTypeId() const
  {
    return GetTypeIdImpl<T>();
  }
};

//==============================================================================
// implementation
//==============================================================================
inline
Entity* Component::GetOwner() const
{
  return m_owner;
}

}

//==============================================================================
///@brief Declares @a name as a derivative of XR::Entity::ComponentT<name> thereby
/// further facilitating the automation of a GetTypeId() implementation required
/// for Component subtypes.
///@usage class or struct XR_COMPONENT_DECL(MyComponent) { /* declarations */ };
///@note Can be combined with templates and multiple inheritance:
/// template <typename T> class XR_COMPONENT_DECL(MyComponent), public OtherBase{};
#define XR_COMPONENT_DECL(name) name: public xr::ComponentT<name>

#endif  //XR_COMPONENT_HPP
