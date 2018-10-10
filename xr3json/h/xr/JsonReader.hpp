#ifndef XR_JSONREADER_HPP
#define XR_JSONREADER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/fundamentals.hpp"
#include "JsonEntity.hpp"
#include "JsonParser.hpp"

namespace xr
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

  ///@brief Parses the given JSON-encoded @a string.
  ///@return  Pointer to root entity. You gain ownership of this object.
  Entity* Read(const char* string, size_t size);

protected:
  // types
  typedef std::vector<Entity*>  EntityVector;

  // data
  Parser          m_parser;
  Parser::String  m_key;
  EntityVector    m_entities;
  Entity*         m_root;

  // internal
  void  OnEntity(Parser::Event e, const Parser::String* string);
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

} // xr

#endif // XR_JSONREADER_HPP
