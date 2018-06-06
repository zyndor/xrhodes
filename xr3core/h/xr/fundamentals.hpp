#ifndef XR_FUNDAMENTALS_HPP
#define XR_FUNDAMENTALS_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include <memory>

//==============================================================================
///@brief Declares a non-object class, a container for all-static members.
#define XR_NONOBJECT_DECL(className) \
  private:\
    className() = delete;\
    ~className() = delete;

//==============================================================================
///@brief Declares a non-copyable class.
#define XR_NONCOPY_DECL(className) \
  private:\
    className(const className& rhs) = delete;\
    className& operator=(const className& rhs) = delete;

//==============================================================================
///@brief Declares the core of a pImpl idiom based class.
#define XR_PIMPL_DECL \
  class Impl;\
  std::unique_ptr<Impl> const m_impl;\

namespace xr
{

//==============================================================================
///@brief Invalid ID to use with XRhodes (after subtracting one from the
/// first valid index to an array).
enum  { INVALID_ID = -1 };

//==============================================================================
///@brief
template  <typename T>
class InstCounted
{
public:
  // static
  static int  GetInstanceCount();

  // structors
  InstCounted();

private:
  // static
  static int  s_instanceCount;
};

//==============================================================================
// implementation
//==============================================================================
template  <typename T>
int InstCounted<T>::s_instanceCount = 0;

//==============================================================================
template  <typename T>
int InstCounted<T>::GetInstanceCount()
{
  return s_instanceCount;
}

//==============================================================================
template  <typename T>
InstCounted<T>::InstCounted()
{
  ++s_instanceCount;
}

}  // xr

#endif  // XR_FUNDAMENTALS_HPP
