#ifndef XR_FILEWRITER_HPP
#define XR_FILEWRITER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================

#include "File.hpp"
#include "xr/types/fundamentals.hpp"

namespace xr
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
  ///@brief Opens a file at @a path for writing in the given @a mode, as
  /// @a text, optionally (as binary if false).
  ///@return The success of the operation.
  bool Open(FilePath const& path, Mode mode, bool text);

  ///@brief Attempts to write @a numElems * @a elemSize bytes of @a buffer, to
  /// the file.
  ///@returns Whether it's succeeded.
  bool Write(void const* buffer, size_t elemSize, size_t numElems);

  ///@brief Writes an object to the buffer.
  ///@note Not for aggregate types with pointer members.
  template <typename T>
  bool Write(T const& val)
  {
    return Write(&val, sizeof(val), 1);
  }

  ///@brief Safeguard against accidentally trying to write pointers.
  template <typename T>
  bool Write(T const* val) = delete;

  ///@return The size of the File being written.
  size_t GetSize() const;

  ///@brief Closes the file. Called automatically when FileWriter goes out of
  /// scope.
  void Close();

private:
  // data
  File::Handle m_handle = nullptr;
};

}

#endif //XR_FILEWRITER_HPP
