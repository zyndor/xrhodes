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
#include "XR/Callback.hpp"
#include "XR/ParserCore.hpp"

namespace XR
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
    const char* pString;
    size_t      length;
  };

  typedef void(*Callback)(Event e, const String* data, void* pUser);

  // structors
  explicit Parser(int maxDepth = kMaxParseDepthDefault);
  ~Parser();

  // general
  ///@brief Sets a callback to call when the maximum depth has been exceeded.
  /// The parser stops raising entity events past the max depth until the
  /// depth is back below the limit. The system parameter of the callback is
  /// a pointer to a boolean to indicate whether the max depth was exceeded.
  void SetMaxDepthCallback(XR::Callback pCb, void* pCbUser);

  ///@return The internal state.
  const ParserCore& GetState() const;

  ///@brief Parses the string in @a parBuffer,
  bool  Parse(const char* parBuffer, size_t size, Callback pCallback, void* pUser);

private:
  // data
  ParserCore    m_state;
  int           m_depth;
  int           m_maxDepth;
  Callback      m_pCallback;
  void*         m_pCallbackUser; // no ownership
  XR::Callback  m_pMaxDepthCallback;
  void*         m_pMaxDepthCallbackUser; // no ownership

  // internal
  bool  _ParseArray();
  bool  _ParseObject();
  bool  _ParseValue();
  void  _DoCallback(Event e, const String* pData);

  void  _IncreaseDepth();
  void  _DecreaseDepth();
  void  _DoDepthCallback(bool exceeded);
};

//==============================================================================
// implementation
//==============================================================================
inline
const ParserCore& Parser::GetState() const
{
  return m_state;
}

} // JSON
} // XR

#endif // XR_JSONPARSER_HPP
