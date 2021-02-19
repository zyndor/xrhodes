#ifndef XR_XONREADER_HPP
#define XR_XONREADER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "XonEntity.hpp"
#include "xr/io/StructuredReader.hpp"
#include "xr/strings/stringutils.hpp"
#include "xr/types/fundamentals.hpp"
#include <vector>

namespace xr
{
namespace detail
{
struct XonReaderInTraits
{
  using Type = XonEntity;

  // static
  static Type const* GetChild(Type const& in, std::string const& key)
  {
    return in.ToObject().TryGet(key);
  }
};
}

//==============================================================================
///@brief Structured reader for processing XON.
template <class Out>
using XonReader = Structured::Reader<detail::XonReaderInTraits, Out>;

//==============================================================================
///@brief Helper functions to interpret XON into various data types.
class XonRead
{
  XR_NONOBJECT_DECL(XonRead)

public:
  ///@brief Attempts to interpret @a xon as a number of type T.
  /// Throws XonEntity::Exception if @a xon is not a value, or std::illegal_argument
  /// if the xonversion has failed.
  template <typename T>
  static T Number(XonEntity const& xon);

  ///@brief Attempts to interpret @a xon as a c-string.
  /// Throws XonEntity::Exception if @a xon is not a value.
  static char const* CString(XonEntity const& xon);

  ///@brief Attempts to interpret @a xon as a std::string.
  /// Throws XonEntity::Exception if @a xon is not a value.
  static std::string String(XonEntity const& xon);

  ///@brief Attempts to interpret @a xon as an array of objects type T.
  /// Throws XonEntity::Exception if @a xon is not an object, or if the underlying
  /// interpreter had failed.
  template <typename T, T(*interpreter)(XonEntity const&)>
  static std::vector<T> Array(XonEntity const& xon);
};

//==============================================================================
// implementation
//==============================================================================
template <typename T>
T XonRead::Number(XonEntity const& xon)
{
  T result;
  auto value = xon.ToValue().GetString();
  if (!StringTo(value, result))
  {
    throw std::invalid_argument(std::string("Xonversion failed: ") + __FUNCTION__ +
      "(" + std::string(value) + ")");
  }
  return result;
}

//==============================================================================
template <typename T, T(*interpreter)(XonEntity const&)>
std::vector<T> XonRead::Array(XonEntity const& xon)
{
  auto& xonObj = xon.ToObject();
  std::vector<T> result;
  result.reserve(xonObj.GetNumElements());
  for (auto& x : xonObj)
  {
    result.push_back(interpreter(*x));
  }
  return result;
}

} // xr

#endif //XR_XONREADER_HPP
