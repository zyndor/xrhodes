//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_SAMPLEDATA_HPP
#define XR_SAMPLEDATA_HPP

#include "types.hpp"
#include "SampleFormat.hpp"

namespace XR
{

//==============================================================================
class  SampleData 
{
public:
  // types
  struct  RiffHeader 
  {
    char    arHead[4];  // "RIFF" expected
    uint32  fileSize;
    char    arSubType[4]; // "WAVE" expected
  };

  struct  RiffChunkHeader 
  {
    char    arType[4];
    uint32  length;
  };

  // data
  int         bufferSize;
  int         numSamples;
  int         sampleRate;
  int         samplesPerBlock;  // adpcm only
  int         blockAlign;       // adpcm only
  uint8*      parSamples;

  // structors
  SampleData();
  ~SampleData();

  // general  
  bool  Load(const char* pFilename);
  
  // internal
  void  _SetBufferSize(int numBytes);
  bool  _ReadChunkFormat(const RiffChunkHeader& rch, int hFile, SampleFormat& sf);
  bool  _ReadChunkData(const RiffChunkHeader& rch, SampleFormat sf, int hFile);
  bool  _ReadChunkFact(const RiffChunkHeader& rch, int hFile);
};

} // XR

#endif // XR_SAMPLEDATA_HPP