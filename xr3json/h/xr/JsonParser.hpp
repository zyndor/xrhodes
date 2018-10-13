#ifndef XR_JSONPARSER_HPP
#define XR_JSONPARSER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "json.hpp"
#include "xr/Callback.hpp"
#include "xr/ParserCore.hpp"
#include <memory>

namespace xr
{
namespace JSON
{

//==============================================================================
class Parser
{
public:
  // types
  enum  Event
  {
    E_KEY,
    E_VALUE,
    E_ARRAY_BEGIN,
    E_ARRAY_END,
    E_OBJECT_BEGIN,
    E_OBJECT_END
  };

  struct  String
  {
    const char* string;
    size_t      length;
  };

  using EntityCallback = Callback<void, Event, const String*>;
  using AtMaxDepthCallback = Callback<void, bool>;

  // structors
  explicit Parser(int maxDepth = kMaxParseDepthDefault);
  ~Parser();

  // general
  ///@brief Sets a callback to call when the maximum depth has been exceeded.
  /// The parser stops raising entity events past the max depth until the
  /// depth is back below the limit. The system parameter of the callback is
  /// a pointer to a boolean to indicate whether the max depth was exceeded.
  void SetAtMaxDepthCallback(AtMaxDepthCallback const& onMaxDepthReachedcb);

  ///@return The internal state.
  const ParserCore& GetState() const;

  ///@brief Parses the string in @a parBuffer,
  bool  Parse(const char* string, size_t size, EntityCallback const& handler);

private:
  // data
  ParserCore m_state;
  int m_depth;
  int m_maxDepth;

  std::unique_ptr<EntityCallback> m_handler;
  std::unique_ptr<AtMaxDepthCallback> m_onAtMaxDepth;

  // internal
  bool  ParseArray();
  bool  ParseObject();
  bool  ParseValue();
  void  OnEntity(Event e, const String* data);

  void  IncreaseDepth();
  void  DecreaseDepth();
  void  DoAtMaxDepth(bool exceeded);
};

//==============================================================================
// implementation
//==============================================================================
inline
void Parser::SetAtMaxDepthCallback(AtMaxDepthCallback const& onMaxDepthReached)
{
  m_onAtMaxDepth.reset(static_cast<AtMaxDepthCallback*>(onMaxDepthReached.Clone()));
}

//==============================================================================
inline
const ParserCore& Parser::GetState() const
{
  return m_state;
}

} // JSON
} // xr

#endif // XR_JSONPARSER_HPP
