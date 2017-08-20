//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_FILEWRITER_HPP
#define XR_FILEWRITER_HPP

#include "File.hpp"
#include <XR/fundamentals.hpp>

namespace XR
{

//==============================================================================
///@brief Facilitates writing to a file.
class FileWriter
{
  XR_NONCOPY_DECL(FileWriter)

public:
  // types
  enum class Mode
  {
    Truncate,
    Append,
  };

  // structors
  FileWriter() = default;
  ~FileWriter();

  // general
  bool Open(File::Path const& path, Mode mode, bool text);
  bool Write(void const* buffer, size_t elemSize, size_t numElems);
  size_t GetSize() const;

  void Close();

private:
  // data
  File::Handle m_handle = nullptr;
};

}

#endif //XR_FILEWRITER_HPP
