#ifndef  XR_STRINGFORMATTER_HPP
#define  XR_STRINGFORMATTER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/debug.hpp"
#include <sstream>

namespace  xr
{

//=============================================================================
///@brief Provides string formatting capabilities reminiscent of printf()'s
/// style but with reorderable, type-agnostic tokens that may occur multiple
/// times. Tokens may also generate subsequent tokens. Up to 9 substitutors
/// are supported (%1 - %9).
class  StringFormatter
{
public:
  // structors
  explicit StringFormatter(char const* cStr)
  : m_string(std::string(cStr))
  {}

  explicit StringFormatter(std::string const& str)
  : m_string(str)
  {}

  explicit StringFormatter(std::string&& str)
  : m_string(str)
  {}

  // general
  const std::string& GetString() const
  {
    return m_string;
  }

  // operators
  template  <typename T>
  StringFormatter&  operator%(T const& value)
  {
    const auto sub = GetNextSubstitutor();

    std::ostringstream  buffer;
    std::string todo{ std::move(m_string) };
    auto iFind = todo.find(sub.buffer);
    while (iFind != std::string::npos)
    {
      buffer << todo.substr(0, iFind);
      buffer << value;

      todo = todo.substr(iFind + sub.size);
      iFind = todo.find(sub.buffer);
    }

    buffer << todo;
    buffer.str().swap(m_string);

    return *this;
  }

  operator std::string() const
  {
    return m_string;
  }

private:
  // types
  struct Substitutor
  {
    char    buffer[4];
    size_t  size;

    explicit Substitutor(int n)
    : size(snprintf(buffer, sizeof(buffer), "%%%d", n))
    {}
  };

  // data
  std::string  m_string;
  int          m_subs = 0;

  // internal
  Substitutor GetNextSubstitutor()
  {
    XR_ASSERTMSG(StringFormatter, m_subs < 9,
      ("The maximum number of substitutors has been used."));
    ++m_subs;

    return Substitutor(m_subs);
  }
};

}  // xr

#endif  //XR_STRINGFORMATTER_HPP
