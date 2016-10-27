//
// Nuclear Heart Games
// XRhodes
//
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
//
// copyright (c) 2011 - 2016. All rights reserved.
//
//==============================================================================
#include <XR/Sample.hpp>
#include <XR/File.hpp>
#include <SDL_mixer.h>

#define TO_IMPL() (static_cast<Mix_Chunk*>(m_pImpl))
#define TO_CONST_IMPL() (static_cast<const Mix_Chunk*>(m_pImpl))

namespace XR
{

XR_CROSS_OBJECT_DEF(Sample, Mix_Chunk)

int Sample::GetNumSamples() const
{
  return TO_CONST_IMPL()->alen;
}

void* Sample::GetData() const
{
  return TO_CONST_IMPL()->abuf;
}

bool Sample::Load( const char* pName )
{
  Mix_Chunk*  pChunk(Mix_LoadWAV(pName));
  bool  result(pChunk != 0);
  void* pImpl(SwapImpl(pChunk));
  return result;
}

} // XR
