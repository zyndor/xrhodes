#include "JsonReader.hpp"
#include "File.hpp"
#include "utils.hpp"

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
  XR_ASSERT(Reader, parBuffer != 0);
  XR_ASSERT(Reader, size >= 0);
  m_key.pString = 0;
  m_key.length = 0;
  m_entities.clear();
  m_pRoot = 0;
  if(!m_parser.Parse(parBuffer, size, OnParserEvent, this))
  {
    delete m_pRoot;
    m_pRoot = 0;
  }

  return m_pRoot;
}

//==============================================================================
void  Reader::_HandleEvent(Parser::Event e, const Parser::String* pString)
{
  Entity* pParent(m_entities.size() > 0 ? m_entities.back() : 0);
  switch(e)
  {
  case  Parser::E_KEY:
    m_key = *pString;
    break;

  case  Parser::E_VALUE:
    {
      XR_ASSERT(Reader, pParent != 0);
      XR_ASSERT(Reader, pParent->GetType() != VALUE);
      XR_ASSERT(Reader, m_key.length > 0);
      XR_ASSERT(Reader, m_key.pString != 0);
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

      if(pParent == 0)
      {
        XR_ASSERT(Reader, m_pRoot == 0);
        m_pRoot = pObject;
      }
      else
      {
        switch(pParent->GetType())
        {
        case  OBJECT:
          XR_ASSERT(Reader, m_key.length > 0);
          XR_ASSERT(Reader, m_key.pString != 0);
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
      XR_ASSERT(Reader, m_entities.size() > 0);
      XR_ASSERT(Reader, m_entities.back()->GetType() == OBJECT);
      m_entities.pop_back();
    }
    break;

  case  Parser::E_ARRAY_BEGIN:
    {
      Array* pArray = new Array();
      m_entities.push_back(pArray);

      if(pParent == 0)
      {
        XR_ASSERT(Reader, m_pRoot == 0);
        m_pRoot = pArray;
      }
      else
      {
        switch(pParent->GetType())
        {
        case  OBJECT:
          XR_ASSERT(Reader, m_key.length > 0);
          XR_ASSERT(Reader, m_key.pString != 0);
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
      XR_ASSERT(Reader, m_entities.size() > 0);
      XR_ASSERT(Reader, m_entities.back()->GetType() == ARRAY);
      m_entities.pop_back();
    }
    break;
  }
}

} // JSON

//==============================================================================
JSON::Entity* LoadJSON(const char* pFilename, int maxDepth, bool quietErrors)
{
  XR_ASSERT(LoadJSON, pFilename != 0);
  int hFile(File::Open(pFilename, "rb"));
  if (hFile == File::INVALID_HANDLE)
  {
    if(!quietErrors)
    {
      XR_ERROR(("Failed to open file '%s'", pFilename));
    }
    return 0;
  }
  
  uint32  size(File::GetSize(hFile));
  char*   parBuffer = new char[size];

  bool  result(File::Read(hFile, size, 1, parBuffer) != 0);
  File::Close(hFile);
  
  if (!result)
  {
    if(!quietErrors)
    {
      XR_ERROR(("Failed to read contents of '%s': %s", pFilename,
        File::GetErrorString()));
    }
    delete[] parBuffer;
    return 0;
  }
  
  // parse parse parse
  JSON::Reader  reader(maxDepth);
  JSON::Entity* pJson(reader.Read(parBuffer, size));
  
  // done, clean up
  delete[] parBuffer;
  if (pJson == 0 && !quietErrors)
  {
    XR_ERROR(("Failed to parse '%s': error around row %d char %d",
      pFilename, reader.GetState().GetRow(), reader.GetState().GetColumn()));
  }
  
  return pJson;
}

} // XR
