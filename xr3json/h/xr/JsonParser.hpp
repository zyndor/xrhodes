#ifndef XR_JSONPARSER_HPP
#define XR_JSONPARSER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "json.hpp"
#include "xr/Callback.hpp"
#include "xr/ParserCore.hpp"

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

  typedef void(*Callback)(Event e, const String* data, void* userData);

  // structors
  explicit Parser(int maxDepth = kMaxParseDepthDefault);
  ~Parser();

  // general
  ///@brief Sets a callback to call when the maximum depth has been exceeded.
  /// The parser stops raising entity events past the max depth until the
  /// depth is back below the limit. The system parameter of the callback is
  /// a pointer to a boolean to indicate whether the max depth was exceeded.
  void SetMaxDepthReachedCallback(xr::Callback onmaxDepthReachedcb, void* userData);

  ///@return The internal state.
  const ParserCore& GetState() const;

  ///@brief Parses the string in @a parBuffer,
  bool  Parse(const char* string, size_t size, Callback callback, void* userData);

private:
  // data
  ParserCore    m_state;
  int           m_depth;
  int           m_maxDepth;
  Callback      m_callback;
  void*         m_callbackUser; // no ownership
  xr::Callback  m_onMaxDepthReached;
  void*         m_onMaxDepthReachedData; // no ownership

  // internal
  bool  _ParseArray();
  bool  _ParseObject();
  bool  _ParseValue();
  void  _DoCallback(Event e, const String* data);

  void  _IncreaseDepth();
  void  _DecreaseDepth();
  void  _DoDepthCallback(bool exceeded);
};

//==============================================================================
// implementation
//==============================================================================
inline
void Parser::SetMaxDepthReachedCallback(xr::Callback onMaxDepthReached, void * userData)
{
  m_onMaxDepthReached = onMaxDepthReached;
  m_onMaxDepthReachedData = userData;
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