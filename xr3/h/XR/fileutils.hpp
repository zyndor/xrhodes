#ifndef XR_FILEUTILS_HPP
#define XR_FILEUTILS_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include <iterator>
#include "File.hpp"

namespace XR
{

template <typename Container>
bool  Write(const Container& c, int hFile);

template <typename Container>
bool  WriteVector(const Container& c, int hFile);

template <typename Container>
bool  Read(int hFile, Container& c);

template <typename Container>
bool  ReadVector(int hFile, Container& c);

//==============================================================================
template <typename Container>
bool  Write(const Container& c, int hFile)
{
  size_t  size(c.size());
  bool  success = File::Write(&size, sizeof(size), 1, hFile) == 1;
  for(Container::const_iterator i0 = c.begin(), i1 = c.end(); success && i0 != i1; ++i0)
  {
    success = File::Write(&*i0, sizeof(typename Container::value_type), 1,
      hFile) == 1;
  }
  return success;    
}


//==============================================================================
template <typename Container>
bool  WriteVector(const Container& c, int hFile)
{
  size_t  size(c.size());
  bool  success = File::Write(&size, sizeof(size), 1, hFile) == 1 &&
    File::Write(&c[0], sizeof(typename Container::value_type), size, hFile) ==
    size;
  return success;    
}

//==============================================================================
template <typename Container>
bool  Read(int hFile, Container& c)
{
  size_t  size;
  bool  success = File::Read(hFile, sizeof(size), 1, &size) == 1;
  while(success && size > 0)
  {
    typename Container::value_type v;
    success = File::Read(hFile, sizeof(typename Container::value_type), 1,
      &v) == 1;
    if(success)
    {
      c.insert(c.end(), v);
    }
    --size;
  }
  return success;    
}


//==============================================================================
template <typename Container>
bool  ReadVector(int hFile, Container& c)
{
  size_t  size;
  bool  success = File::Read(hFile, sizeof(size), 1, &size) == 1;
  if(success)
  {
    c.resize(size);
    success = File::Read(hFile, sizeof(typename Container::value_type), size,
      &c[0]) == size;
  }

  return success;    
}

} // XR

#endif  //XR_FILEUTILS_HPP