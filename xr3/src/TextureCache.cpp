//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/TextureCache.hpp"
#include "xr/AABB.hpp"

namespace xr
{

//==============================================================================
TextureCache::FormatSpec TextureCache::GetFormatSpec(Format f)
{
  FormatSpec fs[] =
  {
    { 1, Gfx::TextureFormat::R8 },
    { 4, Gfx::TextureFormat::RGBA8 }
  };
  return fs[static_cast<int>(f)];
}

//==============================================================================
TextureCache::TextureCache(uint32_t size, Format format, uint32_t blockWidth,
  uint32_t rowHeight)
: m_format(GetFormatSpec(format)),
  m_size(size),
  m_blockWidth(blockWidth),
  m_rowHeight(rowHeight),
  m_pitch(size * m_format.stride),
  m_rowStride(m_pitch * rowHeight),
  m_rows(size / rowHeight),
  m_blocks((m_pitch / blockWidth) * m_rows.size()),
  m_allocs(m_blocks.size(), { nullptr, nullptr, nullptr }),
  m_allocsEnd(m_allocs.data() + m_allocs.size()),
  m_allocsHead(m_allocs.data()),
  m_numAllocs(0),
  m_buffer(size * m_pitch)
{
  XR_ASSERT(TextureCache, m_allocs.size() < std::numeric_limits<AllocId>::max());
  Block* b = m_blocks.data();
  auto last = (m_pitch / blockWidth) - 1;
  for (auto& r : m_rows)
  {
    r.blocks = b;
    r.firstAvailable = b;

    std::fill(b, b + last, Block{ false, 0 });
    b += last;
    *b = Block{ true , 0 };
    ++b;
  }
}

//==============================================================================
uint8_t* TextureCache::Allocate(uint32_t widthPixels, uint32_t heightPixels,
  AABB& uvs)
{
  XR_ASSERTMSG(TextureCache, widthPixels <= m_size,
    ("Allocation of %d pixels is greater than row size (%d pixels); resize cache.",
      widthPixels, m_size));
  XR_ASSERTMSG(TextureCache, heightPixels <= m_rowHeight,
    ("Allocation (%d) is taller than row height (%d); reisze cache.", heightPixels,
      m_rowHeight));

  Allocation* alloc = nullptr;
  Row* row = nullptr;
  Block* block = nullptr;
  Block* base = nullptr;

  // NOTE: even a 0 pixel allocation will result in a block being used.
  // MORAL: don't go allocating chunks that are 0 pixels.
  const uint32_t needBlocks = 1 + (widthPixels * m_format.stride) / m_blockWidth;

  // Search the rows for a large enough free chunk.
  for (auto& r : m_rows)
  {
    auto iBlock = r.firstAvailable;
    if (iBlock)
    {
      base = iBlock;
      uint32_t gotBlocks = 0;
      while (gotBlocks < needBlocks)
      {
        if (iBlock->allocSize == 0)
        {
          ++gotBlocks;
          if (iBlock->last)
          {
            break;  // while()
          }
          ++iBlock;
        }
        else  // not consecutive, skip and start over
        {
          gotBlocks = 0;
          if (iBlock->last)
          {
            break;  // while()
          }
          iBlock = iBlock->Skip();
          XR_ASSERT(TextureCache, !(iBlock-1)->last);
          base = iBlock;
        }
      }

      if (gotBlocks == needBlocks) // got ourselves an allocation, yay.
      {
        row = &r;
        block = base;
        break;  // for()
      }
    }
  }

  uint8_t* buffer = nullptr;
  if (block)
  {
    XR_ASSERT(TextureCache, row);
    block->allocSize = needBlocks;
    if (block == row->firstAvailable) // move first ptr on.
    {
      auto next = block->Skip();
      if ((next - 1)->last)
      {
        row->firstAvailable = nullptr;
      }
      else
      {
        row->firstAvailable = next;
      }
    }

    alloc = &Next();
    alloc->block = block;
    alloc->row = row;

    ptrdiff_t y = row - m_rows.data();
    ptrdiff_t x = block - row->blocks;
    buffer = m_buffer.data() + y * m_rowStride + x * m_blockWidth;
    alloc->buffer = buffer;

    ptrdiff_t u = x * m_blockWidth;
    uvs.left = u / float(m_size);
    uvs.right = (u + widthPixels) / float(m_size);

    ptrdiff_t v = y * m_rowHeight;
    uvs.top = v / float(m_size);
    uvs.bottom = (v + heightPixels) / float(m_size);

    XR_ASSERT(TextureCache, alloc->buffer >= m_buffer.data());
    XR_ASSERT(TextureCache, alloc->buffer <= m_buffer.data() + m_rowStride * m_rows.size());
  }

  return buffer;
}

//==============================================================================
Gfx::TextureHandle TextureCache::UpdateTexture(uint32_t flags)
{
  Buffer buffer = { m_buffer.size(), m_buffer.data() };
  return Gfx::CreateTexture(m_format.textureFormat, m_size, m_size, 0, flags, &buffer);
}

//==============================================================================
void TextureCache::Deallocate(uint8_t* buffer)
{
  // linear search allocations for the given buffer, from head to end, then from start to last alloc.
  auto findPredicate = [buffer](Allocation const& a) {
    return a.buffer == buffer;
  };
  auto iFind = std::find_if(m_allocsHead, m_allocsEnd, findPredicate);
  if(iFind != m_allocsEnd)
  {
    Deallocate(static_cast<AllocId>(iFind - m_allocs.data()));
  }
  else
  {
    iFind = std::find_if(m_allocs.data(), m_allocs.data() + (m_numAllocs - (m_allocsEnd - m_allocsHead)), findPredicate);
    if(iFind != m_allocsEnd)
    {
      Deallocate(static_cast<AllocId>(iFind - m_allocs.data()));
    }
  }
}

//==============================================================================
void TextureCache::Reset()
{
  auto i = m_allocsHead;
  while (i != m_allocsEnd)
  {
    i->Reset();
    ++i;
  }

  i = m_allocs.data();
  auto iEnd = i + (m_numAllocs - (m_allocsEnd - m_allocsHead));
  while (i != iEnd)
  {
    i->Reset();
    ++i;
  }

  m_numAllocs = 0;
  m_allocsHead = m_allocs.data();
}

//==============================================================================
TextureCache::Allocation& TextureCache::Next()
{
  auto const base = m_allocs.data();
  auto& alloc = *(base + ((m_allocsHead - base) + m_numAllocs) % m_allocs.size());
  if (m_numAllocs < m_allocs.size())
  {
    ++m_numAllocs;
  }
  else
  {
    ++m_allocsHead;
    if (m_allocsHead == m_allocsEnd)
    {
      m_allocsHead = base;
    }

    alloc.Reset();
  }
  return alloc;
}

//==============================================================================
void TextureCache::Deallocate(AllocId id)  // absolute index into the array of allocations
{
  // Swaps our guy with the most recent allocation, then pops it off.
  auto base = m_allocs.data();
  auto iHead = m_allocsHead - base;
  auto iLast = (iHead + m_numAllocs - 1) % m_allocs.size();
  auto& alloc = base[iLast];
  if (id != iLast) // unless the deallocated entry is the last one, swap it.
  {
    std::swap(alloc, base[id]);
  }

  alloc.Reset();
  --m_numAllocs;
}

}
