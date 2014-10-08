//
// Nuclear Heart Games
// XRhodes
//
// JsonReader.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    04/01/2014
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_JSONREADER_HPP
#define XR_JSONREADER_HPP

#include "JsonEntity.hpp"
#include "JsonParser.hpp"

namespace XR
{
namespace JSON
{

//==============================================================================
///@brief Attempts to parse a string as JSON and creates entity hierarchy from
/// it.
class  Reader
{
  XR_NONCOPY_DECL(Reader)

public:
  // structors
  explicit Reader(int maxDepth = kMaxParseDepthDefault);
  ~Reader();
  
  // general
  const ParserCore& GetState() const;
  
  ///@brief Parses the string in @a parBuffer.
  ///@return  Pointer to root entity. You gain ownership of this object.
  Entity* Read(const char* parBuffer, int size);
  
protected:
  // types
  typedef std::vector<Entity*>  EntityVector;

  // static
  static void OnParserEvent(Parser::Event e, const Parser::String* pString,
                void* pUser);
  
  // data
  Parser          m_parser;
  Parser::String  m_key;   
  EntityVector    m_entities;
  Entity*         m_pRoot;
  
  // internal
  void  _HandleEvent(Parser::Event e, const Parser::String* pString);
};

//==============================================================================
// implementation
//==============================================================================
inline
const ParserCore& Reader::GetState() const
{
  return m_parser.GetState();
}

} // JSON

//==============================================================================
JSON::Entity* LoadJSON(const char* pFilename,
          int maxDepth = JSON::kMaxParseDepthDefault, bool quietErrors = false);

} // XR

#endif // XR_JSONREADER_HPP