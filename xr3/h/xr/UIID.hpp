#ifndef XR_UIID_HPP
#define XR_UIID_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================

#include <vector>
#include <cstdint>

namespace xr
{

//==============================================================================
///@brief 16-byte Unique Instance Identifier.
//==============================================================================
struct UIID
{
  // types
  using value_type = uint8_t;
  using iterator = value_type*;
  using const_iterator = value_type const*;

  // static
  static constexpr uint32_t kSize = 16;

  ///@brief Generates a UIID.
  static UIID Generate();

  // general
  [[deprecated]]
  size_t capacity() { return kSize; }

  [[deprecated]]
  size_t size() { return kSize; }

  value_type* data() { return mData; }

  const_iterator begin() const { return mData; }
  const_iterator end() const { return mData + kSize; }

  iterator begin() { return mData; }
  iterator end() { return mData + kSize; }

  // operator overloads
  value_type operator[](size_t idx) const { return mData[idx]; }
  value_type& operator[](size_t idx) { return mData[idx]; }

private:
  // data
  value_type mData[kSize];

  // structors
  UIID() = default;
};

[[deprecated("Use UIID::kSize")]]
extern const size_t kUIIDSize;

[[deprecated("Use UIID::Generate()")]]
inline
UIID  GenerateUIID()
{
  return UIID::Generate();
}

} // xr

#endif  //XR_UIID_HPP
