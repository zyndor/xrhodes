//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/JsonReader.hpp"
#include "xr/debug.hpp"

namespace xr
{
namespace JSON
{

//==============================================================================
void  Reader::OnParserEvent(Parser::Event e, const Parser::String* string,
  void* userData)
{
  static_cast<Reader*>(userData)->_HandleEvent(e, string);
}

//==============================================================================
Reader::Reader(int depth)
: m_parser(depth),
  m_key(),
  m_entities(depth),
  m_root(nullptr)
{}

//==============================================================================
Reader::~Reader()
{}

//==============================================================================
Entity* Reader::Read(const char* string, size_t size)
{
  XR_ASSERT(Json::Reader, string != nullptr);
  XR_ASSERT(Json::Reader, size >= 0);
  m_key.string = nullptr;
  m_key.length = 0;
  m_entities.clear();
  m_root = nullptr;
  if(!m_parser.Parse(string, size, OnParserEvent, this))
  {
    delete m_root;
    m_root = nullptr;
  }

  return m_root;
}

//==============================================================================
void  Reader::_HandleEvent(Parser::Event e, const Parser::String* string)
{
  Entity* parent(m_entities.size() > 0 ? m_entities.back() : nullptr);
  switch(e)
  {
  case  Parser::E_KEY:
    m_key = *string;
    break;

  case  Parser::E_VALUE:
    {
      XR_ASSERT(Json::Reader, parent != nullptr);
      XR_ASSERT(Json::Reader, parent->GetType() != VALUE);
      XR_ASSERT(Json::Reader, m_key.length > 0);
      XR_ASSERT(Json::Reader, m_key.string != nullptr);
      Entity* value(new Value(string->string, string->length));
      switch(parent->GetType())
      {
      case  OBJECT:
        parent->ToObject()->AddChild(m_key.string, m_key.length, value);
        break;

      case  ARRAY:
        parent->ToArray()->AddElement(value);
        break;

      default:
        XR_ERROR(("Invalid parent type."));
        break;
      }
    }
    break;

  case  Parser::E_OBJECT_BEGIN:
    {
      Object* object = new Object();
      m_entities.push_back(object);

      if(parent == nullptr)
      {
        XR_ASSERT(Json::Reader, m_root == nullptr);
        m_root = object;
      }
      else
      {
        switch(parent->GetType())
        {
        case  OBJECT:
          XR_ASSERT(Json::Reader, m_key.length > 0);
          XR_ASSERT(Json::Reader, m_key.string != nullptr);
          parent->ToObject()->AddChild(m_key.string, m_key.length, object);
          break;

        case  ARRAY:
          parent->ToArray()->AddElement(object);
          break;

        default:
          XR_ERROR(("Invalid parent type."));
          break;
        }
      }
    }
    break;

  case  Parser::E_OBJECT_END:
    {
      XR_ASSERT(Json::Reader, m_entities.size() > 0);
      XR_ASSERT(Json::Reader, m_entities.back()->GetType() == OBJECT);
      m_entities.pop_back();
    }
    break;

  case  Parser::E_ARRAY_BEGIN:
    {
      Array* array = new Array();
      m_entities.push_back(array);

      if(parent == nullptr)
      {
        XR_ASSERT(Json::Reader, m_root == nullptr);
        m_root = array;
      }
      else
      {
        switch(parent->GetType())
        {
        case  OBJECT:
          XR_ASSERT(Json::Reader, m_key.length > 0);
          XR_ASSERT(Json::Reader, m_key.string != nullptr);
          parent->ToObject()->AddChild(m_key.string, m_key.length, array);
          break;

        case  ARRAY:
          parent->ToArray()->AddElement(array);
          break;

        default:
          XR_ERROR(("Invalid type."));
          break;
        }
      }
    }
    break;

  case  Parser::E_ARRAY_END:
    {
      XR_ASSERT(Json::Reader, m_entities.size() > 0);
      XR_ASSERT(Json::Reader, m_entities.back()->GetType() == ARRAY);
      m_entities.pop_back();
    }
    break;
  }
}

} // JSON

} // xr
