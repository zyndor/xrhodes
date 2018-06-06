//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef  XR_STRINGFORMATTER_HPP
#define  XR_STRINGFORMATTER_HPP

#include "debug.hpp"
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
  explicit StringFormatter(char const* string)
  : m_string(string),
    m_subs(0)
  {}

  explicit StringFormatter(std::string const& str)
  : StringFormatter(str.c_str())
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
    auto iFind = m_string.find(sub.buffer);
    size_t stringSize = m_string.size();
    while (iFind != std::string::npos)
    {
      const auto afterSub = iFind + sub.size;
      buffer.str("");
      buffer << m_string.substr(0, iFind) << value << m_string.substr(afterSub);
      buffer.str().swap(m_string);

      // NOTE: we should start looking for the next token at the end of this
      // substitution to avoid the fatal and unrealistic scenario of a token
      // being replaced with itself (part or whole).
      // i.e. in "%1 morning!" % "Good %1" we should not look to replace the
      // %1 generated in the substitution. Note that "%1 morning!" % "Good %2"
      // will work as expected.
      const auto oldStringSize = stringSize;
      stringSize = m_string.size();
      iFind = m_string.find(sub.buffer, afterSub + ptrdiff_t(stringSize - oldStringSize));
    }

    return *this;
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
  int          m_subs;

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
