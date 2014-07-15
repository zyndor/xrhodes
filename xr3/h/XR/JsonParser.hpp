//
// Nuclear Heart Games
// XRhodes
//
// JsonParser.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    14/07/2014
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_JSONPARSER_HPP
#define XR_JSONPARSER_HPP

#include "json.hpp"
#include "ParserCore.hpp"

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
    int         length;
  };
  
  typedef void(*Callback)(Event e, const String* data, void* pUser);
  
  // structors
  explicit Parser(int maxDepth = kMaxParseDepthDefault);
  ~Parser();
  
  // general
  ///@return The internal state.
  const ParserCore& GetState() const;
  
  ///@brief Parses the string in @a parBuffer, 
  bool  Parse(const char* parBuffer, int size, Callback pCallback, void* pUser);
  
private:
  // data
  ParserCore  m_state;
  int         m_depth;
  int         m_maxDepth;
  Callback    m_pCallback;
  void*       m_pCallbackUser;
  
  // internal
  bool  _ParseArray();
  bool  _ParseObject();
  bool  _ParseValue();
  void  _DoCallback(Event e, const String* pData);

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
