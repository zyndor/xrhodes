//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include  "CSV.hpp"
#include  "FileBuffer.hpp"
#include  "Hash.hpp"

namespace XR
{

//==============================================================================
void CSV::CreateMapping(const ConstStringPtrVector& keys,
  const StringPtrVector& values, StringMap& m)
{
  XR_ASSERT(CSV, keys.size() == values.size());
  StringPtrVector::const_iterator iValue(values.begin());
  for(ConstStringPtrVector::const_iterator i0(keys.begin()), i1(keys.end());
    i0 != i1; ++i0)
  {
    uint32  hash(Hash::String((*i0)->c_str()));
    m[hash] = *iValue;
    ++iValue;
  }
}

//==============================================================================
void CSV::CreateMapping(const ConstStringPtrVector& keys,
  const ConstStringPtrVector& values, ConstStringMap& m)
{
  XR_ASSERT(CSV, keys.size() == values.size());
  ConstStringPtrVector::const_iterator iValue(values.begin());
  for(ConstStringPtrVector::const_iterator i0(keys.begin()), i1(keys.end());
    i0 != i1; ++i0)
  {
    uint32  hash(Hash::String((*i0)->c_str()));
    m[hash] = *iValue;
    ++iValue;
  }
}

//==============================================================================
bool  CSV::WriteString(const char* pString, FileBuffer& file)
{
  int   len(strlen(pString));
  const char* pComma(strchr(pString, ','));
  bool  success(true);
  if(pComma != 0)
  {
    success = file.Write("\"", 1, 1);
  }
  
  if(success)
  {
    success = file.Write(pString, len, 1);
  }

  if(success && pComma != 0)
  {
    success = file.Write("\"", 1, 1);
  }

  return success;
}

//==============================================================================
CSV::CSV()
: m_numColumns(-1),
  m_numRows(0),
  m_data()
{}

//==============================================================================
CSV::~CSV()
{}

//==============================================================================
bool  CSV::Load(const char* pFileName)
{
  Clear();

  FileBuffer  file;
  bool  success(file.Open(pFileName, "rb"));
  if(success)
  {
    const char* pRead(file.GetData());
    const char* pEnd(pRead + file.GetSize());
    StringList  sl;
    while(pRead < pEnd && success)
    {
      const char* pLineEnd(pRead + strcspn(pRead, "\n\r"));
      if(pLineEnd > pEnd)
      {
        pLineEnd = pEnd;
      }

      StringList  row;
      success = _ParseLine(pRead, pLineEnd, row);
      if(success)
      {
        sl.splice(sl.end(), row);
        ++m_numRows;
      }

      pRead = pLineEnd;
      while(strcspn(pRead, "\n\r") == 0)
      {
        ++pRead;
      }
    }

    if(success)
    {
      m_data.insert(m_data.end(), sl.begin(), sl.end());
    }
  }

  return success;
}

//==============================================================================
bool  CSV::Save(const char* pFileName, bool overwrite) const
{
  FileBuffer  file;
  bool  success(file.Open(pFileName, "wb"));
  if(success)
  {
    StringVector::const_iterator  iter(m_data.begin());
    XR_ASSERT(CSV, m_data.size() == m_numColumns * m_numRows);
    for(int i = 0; i < m_numRows && success; ++i)
    {
      success = WriteString(iter->c_str(), file);
      ++iter;
      for(int j = 1; j < m_numColumns && success; ++j)
      {
        success = file.Write(",", 1, 1) && WriteString(iter->c_str(), file);
        ++iter;
      }
      success = file.Write("\n", 1, 1 );
    }
  }
  return success;
}

//==============================================================================
const std::string* CSV::Get(int column, int row) const
{
  XR_ASSERT(CSV, column >= 0);
  XR_ASSERT(CSV, column < m_numColumns);
  XR_ASSERT(CSV, row >= 0);
  XR_ASSERT(CSV, row < m_numRows);
  return &m_data[row * m_numColumns + column];
}

//==============================================================================
std::string*  CSV::Get(int column, int row)
{
  XR_ASSERT(CSV, column >= 0);
  XR_ASSERT(CSV, column < m_numColumns);
  XR_ASSERT(CSV, row >= 0);
  XR_ASSERT(CSV, row < m_numRows);
  return &m_data[row * m_numColumns + column];
}

//==============================================================================
void  CSV::GetRow(int row, bool skipHead, ConstStringPtrVector& v) const
{
  XR_ASSERT(CSV, row >= 0);
  XR_ASSERT(CSV, row < m_numRows);
  v.clear();
  int offset(skipHead ? 1 : 0);
  v.reserve(m_numColumns - offset);
  StringVector::const_iterator i0(m_data.begin());
  std::advance(i0, m_numColumns * row + offset);
  StringVector::const_iterator i1(i0);
  std::advance(i1, m_numColumns - offset);
  while(i0 != i1)
  {
    v.push_back(&*i0);
    ++i0;
  }
}

//==============================================================================
void  CSV::GetRow(int row, bool skipHead, StringPtrVector& v)
{
  XR_ASSERT(CSV, row >= 0);
  XR_ASSERT(CSV, row < m_numRows);
  v.clear();
  int offset(skipHead ? 1 : 0);
  v.reserve(m_numColumns - offset);
  StringVector::iterator i0(m_data.begin());
  std::advance(i0, m_numColumns * row + offset);
  StringVector::iterator i1(i0);
  std::advance(i1, m_numColumns - offset);
  while(i0 != i1)
  {
    v.push_back(&*i0);
    ++i0;
  }
}

//==============================================================================
void  CSV::GetColumn(int column, bool skipHead, ConstStringPtrVector& v) const
{
  XR_ASSERT(CSV, column >= 0);
  XR_ASSERT(CSV, column < m_numColumns);
  v.clear();
  int offset(skipHead ? 1 : 0);
  v.reserve(m_numRows - offset);
  StringVector::const_iterator i0(m_data.begin());
  std::advance(i0, column + offset * m_numColumns);
  for(int i = offset; i < m_numRows; ++i)
  {
    v.push_back(&*i0);
    std::advance(i0, m_numColumns);
  }
}

//==============================================================================
void  CSV::GetColumn(int column, bool skipHead, StringPtrVector& v)
{
  XR_ASSERT(CSV, column >= 0);
  XR_ASSERT(CSV, column < m_numColumns);
  v.clear();
  int offset(skipHead ? 1 : 0);
  v.reserve(m_numRows - offset);
  StringVector::iterator i0(m_data.begin());
  std::advance(i0, column + offset * m_numColumns);
  for(int i = offset; i < m_numRows; ++i)
  {
    v.push_back(&*i0);
    std::advance(i0, m_numColumns);
  }
}

//==============================================================================
void  CSV::Clear()
{
  m_numColumns = -1;
  m_numRows = 0;
  m_data.clear();
}

//==============================================================================
bool  CSV::_ParseLine(const char* pStart, const char* pEnd, StringList& sl)
{
  bool  success(true);
  int   columns(0);
  while(pStart < pEnd && success)
  {
    const char* pToken(pStart);
    const bool  quoted(*pStart == '"');
    if(quoted)
    {
      ++pStart;
      ++pToken;
      pToken += strcspn(pStart, "\"");
    }
    else
    {
      pToken += strcspn(pStart, ",");
    }

    if(pToken > pEnd)
    {
      pToken = pEnd;
    }
    sl.push_back(std::string(pStart, pToken - pStart));
    
    if(quoted)
    {
      ++pToken;
    }
    ++pToken;

    pStart = pToken;
    ++columns;
  }

  if(m_numColumns < 0)
  {
    m_numColumns = columns;
  }
  else
  {
    success = (columns == m_numColumns);
  }
  return success;
}


} // XR