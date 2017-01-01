//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_CSV_HPP
#define XR_CSV_HPP

#include  <list>
#include  <vector>
#include  <string>
#include  <map>
#include  "ParserCore.hpp"

namespace XR
{

class FileBuffer;

//==============================================================================
class CSV
{
public:
  // types
  typedef std::list<std::string>                StringList;
  typedef std::vector<std::string>              StringVector;
  typedef std::vector<std::string*>             StringPtrVector;
  typedef std::vector<const std::string*>       ConstStringPtrVector;
  typedef std::map<uint32, std::string*>        StringMap;
  typedef std::map<uint32, const std::string*>  ConstStringMap;

  // static
  static void CreateMapping(const ConstStringPtrVector& keys,
    const StringPtrVector& values, StringMap& m);
  static void CreateMapping(const ConstStringPtrVector& keys,
    const ConstStringPtrVector& values, ConstStringMap& m);

  // structors
  CSV();
  ~CSV();

  // general
  bool                Load(const char* pFileName);
  bool                Save(const char* pFileName, bool overwrite) const;

  int                 GetNumColumns() const;
  int                 GetNumRows() const;

  const std::string*  Get(int column, int row) const;
  std::string*        Get(int column, int row);

  void                GetRow(int row, bool skipHead, ConstStringPtrVector& v) const;
  void                GetRow(int row, bool skipHead, StringPtrVector& v);

  void                GetColumn(int column, bool skipHead, ConstStringPtrVector& v) const;
  void                GetColumn(int column, bool skipHead, StringPtrVector& v);

  void                Clear();

protected:
  // types
  typedef std::map<uint32, ConstStringPtrVector>  StringPtrVectorMap;

  // static
  static bool  WriteString(const char* pString, FileBuffer& file);

  // data
  int                 m_numColumns;
  int                 m_numRows;
  StringVector        m_data;

  // internal
  bool  _ParseLine(const char* pStart, const char* pEnd, StringList& sl);
};

} // XR

#endif  //XR_CSV_HPP