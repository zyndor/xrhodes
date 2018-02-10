#ifndef XR_TEXTURECACHE_HPP
#define XR_TEXTURECACHE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/Gfx.hpp"
#include "XR/debug.hpp"
#include <vector>
#include <cstdint>

namespace XR
{

struct AABB;

//==============================================================================
/// @brief TextureCache creates and manages a buffer split into fixed height
/// rows of multiple blocks. User code may request allocations of variable
/// pixel width (spanning one or more contiguous blocks) that are no larger in
/// height than a row.
class TextureCache
{
public:
  enum class Format
  {
    U8,
    U32
  };

  struct FormatSpec
  {
    uint32_t stride;
    Gfx::TextureFormat textureFormat;
  };

  // static
  static FormatSpec GetFormatSpec(Format f);

  // structors
  TextureCache(uint32_t size, Format format, uint32_t blockWidth, uint32_t rowHeight);

  // general
  ///@return The format of the TextureCache.
  FormatSpec const& GetFormat() const { return m_format; }

  ///@return The number of pixels horizontally and vertically.
  ///@note Pixels may be multiple bytes, which only factors in horizontally;
  /// see GetPitch().
  uint32_t GetSize() const { return m_size; }

  ///@return The number of bytes in one pixel row.
  uint32_t GetPitch() const { return m_pitch; }

  ///@brief Returns pointer to start of allocated buffer, and if an allocation
  /// was made, @a uvsOut will have the uvs for the given section.
  ///@note Buffer is not contiguous; it is (widthPixels * m_format.stride) by
  /// heightPixels, its pitch being GetPitch() bytes.
  uint8_t* Allocate(uint32_t widthPixels, uint32_t heightPixels, AABB& uvsOut);

  ///@brief Creates a texture with the contents of the buffer.
  ///@return The handle to and the ownership of, the texture.
  /// TODO: actual update instead of (re-)creation.
  Gfx::TextureHandle UpdateTexture(uint32_t flags = 0); // ownership transfer

  ///@brief Releases the use of the buffer starting at @a buffer, which must
  /// come from a call to Allocate().
  void Deallocate(uint8_t* buffer);

  ///@brief Clears all allocations.
  void Reset();

  ///@return The pointer to the pixel data.
  uint8_t const* GetBuffer() { return m_buffer.data(); }

private:
  // types
  struct Block
  {
    bool last : 1;
    uint32_t allocSize : 31;

    Block* Skip()
    {
      XR_ASSERT(Font, !last && allocSize > 0); // Should only be used on allocated blocks
      return this + allocSize;
    }
  };

  struct Row
  {
    Block* blocks;
    Block* firstAvailable;
  };

  struct Allocation
  {
    Row* row;
    Block* block;
    uint8_t* buffer;

    void Reset()
    {
      XR_ASSERT(TextureCache, row != nullptr);
      XR_ASSERT(TextureCache, block != nullptr);
      if (!row->firstAvailable || row->firstAvailable > block)
      {
        row->firstAvailable = block;
      }
      block->allocSize = 0;
      block = nullptr;
      row = nullptr;
      buffer = nullptr;
    }
  };

  // data
  FormatSpec m_format;

  uint32_t m_size;
  uint32_t m_blockWidth;
  uint32_t m_rowHeight;

  uint32_t m_pitch; // bytes in a row.
  uint32_t m_rowStride; // m_pitch * m_rowHeight

  std::vector<Row> m_rows;
  std::vector<Block> m_blocks;

  std::vector<Allocation> m_allocs; // circular
  Allocation* m_allocsEnd; // end of buffer
  Allocation* m_allocsHead;
  uint32_t m_numAllocs;

  std::vector<uint8_t> m_buffer;

  // internal
  Allocation& Next();

  void Deallocate(uint32_t id);
};

}

#endif
