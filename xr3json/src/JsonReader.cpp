//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <XR/JsonReader.hpp>
//#include <XR/utils.hpp>
#include <XR/debug.hpp>

namespace XR
{  
namespace JSON
{  

//==============================================================================
void  Reader::OnParserEvent(Parser::Event e, const Parser::String* pString,
        void* pUser)
{
  static_cast<Reader*>(pUser)->_HandleEvent(e, pString);
}

//==============================================================================
Reader::Reader(int depth)
: m_parser(depth),
  m_key(),
  m_entities(depth),
  m_pRoot(0)
{}

//==============================================================================
Reader::~Reader()
{}

//==============================================================================
Entity* Reader::Read(const char* parBuffer, int size)
{
  XR_ASSERT(Json::Reader, parBuffer != nullptr);
  XR_ASSERT(Json::Reader, size >= 0);
  m_key.pString = nullptr;
  m_key.length = 0;
  m_entities.clear();
  m_pRoot = nullptr;
  if(!m_parser.Parse(parBuffer, size, OnParserEvent, this))
  {
    delete m_pRoot;
    m_pRoot = nullptr;
  }

  return m_pRoot;
}

//==============================================================================
void  Reader::_HandleEvent(Parser::Event e, const Parser::String* pString)
{
  Entity* pParent(m_entities.size() > 0 ? m_entities.back() : nullptr);
  switch(e)
  {
  case  Parser::E_KEY:
    m_key = *pString;
    break;

  case  Parser::E_VALUE:
    {
      XR_ASSERT(Json::Reader, pParent != nullptr);
      XR_ASSERT(Json::Reader, pParent->GetType() != VALUE);
      XR_ASSERT(Json::Reader, m_key.length > 0);
      XR_ASSERT(Json::Reader, m_key.pString != 0);
      Entity* pValue(new Value(pString->pString, pString->length));
      switch(pParent->GetType())
      {
      case  OBJECT:
        pParent->ToObject()->AddChild(m_key.pString, m_key.length, pValue);
        break;

      case  ARRAY:
        pParent->ToArray()->AddElement(pValue);
        break;

      default:
        XR_ERROR(("Invalid parent type."));
        break;
      }
    }
    break;

  case  Parser::E_OBJECT_BEGIN:
    {
      Object* pObject = new Object();
      m_entities.push_back(pObject);

      if(pParent == nullptr)
      {
        XR_ASSERT(Json::Reader, m_pRoot == nullptr);
        m_pRoot = pObject;
      }
      else
      {
        switch(pParent->GetType())
        {
        case  OBJECT:
          XR_ASSERT(Json::Reader, m_key.length > 0);
          XR_ASSERT(Json::Reader, m_key.pString != nullptr);
          pParent->ToObject()->AddChild(m_key.pString, m_key.length, pObject);
          break;

        case  ARRAY:
          pParent->ToArray()->AddElement(pObject);
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
      Array* pArray = new Array();
      m_entities.push_back(pArray);

      if(pParent == 0)
      {
        XR_ASSERT(Json::Reader, m_pRoot == nullptr);
        m_pRoot = pArray;
      }
      else
      {
        switch(pParent->GetType())
        {
        case  OBJECT:
          XR_ASSERT(Json::Reader, m_key.length > 0);
          XR_ASSERT(Json::Reader, m_key.pString != nullptr);
          pParent->ToObject()->AddChild(m_key.pString, m_key.length, pArray);
          break;

        case  ARRAY:
          pParent->ToArray()->AddElement(pArray);
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

} // XR