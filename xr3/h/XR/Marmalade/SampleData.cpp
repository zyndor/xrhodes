#include "SampleData.hpp"
#include "SampleFormat.hpp"
#include "Adpcm.hpp"
#include "File.hpp"
#include <cstring>

namespace XR
{

//==============================================================================
struct  WaveFormatAdpcm 
{
  uint16 formatTag;
  uint16 channels;
  uint32 sampleRate;
  uint32 avgBytesPerSec;
  uint16 blockAlign;
  uint16 bitsPerSample;  // 8 or 16 expected
  uint16 cbSize;
  uint16 samplesPerBlock;
};

//==============================================================================
enum
{
  FT_PCM = 1,
  FT_ADPCM = 17,
};

//==============================================================================
SampleData::SampleData()
: bufferSize(0),
  numSamples(0),
  sampleRate(0),
  samplesPerBlock(0),
  blockAlign(0),
  parSamples(0)
{}

//==============================================================================
SampleData::~SampleData()
{
  delete[] parSamples;
  parSamples = 0;
}

//==============================================================================
bool SampleData::Load( const char* pFilename )
{
  int hFile(File::Open(pFilename, "rb"));
  if(hFile == File::INVALID_HANDLE)
  {
    return false;
  }

  uint32 fileSize(File::GetSize(hFile));

  RiffHeader  rh;
  if(File::Read(sizeof(RiffHeader), 1, hFile, &rh) != 1 ||
    strncmp(rh.arHead, "RIFF", 4) != 0 ||
    strncmp(rh.arSubType, "WAVE", 4) != 0) 
  {
    File::Close(hFile);
    return false;
  }

  RiffChunkHeader rch;
  SampleFormat    sf(SF_UNKNOWN);
  bool            error(false);
  bool            readData(false);

  while(File::Read(sizeof(RiffChunkHeader), 1, hFile, &rch) == 1)
  {
    uint32 chunkStart = File::Tell(hFile);

    if(strncmp(rch.arType, "fmt ", 4) == 0)
    {
      error = !_ReadChunkFormat(rch, hFile, sf);
      if(error)
      {
        File::Close(hFile);
        break;
      }
    }
    else if(strncmp(rch.arType, "data", 4) == 0)
    {
      error = !_ReadChunkData(rch, sf, hFile);
      if(error)
      {
        File::Close(hFile);
        break;
      }
      readData = true;
    }
    else if(strncmp(rch.arType, "fact", 4) == 0)
    {
      error = !_ReadChunkFact(rch, hFile);
      if(error)
      {
        File::Close(hFile);
        break;
      }
    }

    if(chunkStart + rch.length > fileSize)
    {
      break;
    }

    File::Seek(hFile, chunkStart + rch.length, File::SF_START);
  }
  
  File::Error fErr(File::GetError());
  error = error || !(fErr == File::ERR_EOF || fErr == File::ERR_NONE);
  
  File::Close(hFile);

  return !error && readData;
}

//==============================================================================
bool SampleData::_ReadChunkFormat( const RiffChunkHeader& rch, int hFile,
  SampleFormat& sf )
{
  WaveFormatAdpcm wf;
  
  if(rch.length < 16 ||
    File::Read(sizeof(WaveFormatAdpcm), 1, hFile, &wf) != 1)
  {
    XR_ERROR(("Invalid format chunk in WAV file %s.", File::GetName(hFile)));
    return false;
  }
  
  if(wf.channels > 1)
  {
    XR_ERROR(("%s has %d channels; max. 1 is supported.",
      File::GetName(hFile), wf.channels));
    return false;
  }
  
  switch(wf.formatTag)
  {
  case FT_PCM: // PCM
    switch(wf.bitsPerSample)
    {
    case 8:
      sf = SF_PCM_8_MONO;
      break;

    case 16:
      sf = SF_PCM_16_MONO;
      break;
      
    default:
      XR_ERROR(("%s has unsupported bits-per-sample (%d)", File::GetName(hFile),
        wf.bitsPerSample));
      return false;
    }
    break;
    
  case FT_ADPCM:  // ADPCM
    sf = SF_ADPCM_MONO;
    samplesPerBlock = wf.samplesPerBlock;
    blockAlign = wf.blockAlign;
    break;
    
  default:
    XR_ERROR(("Unsupported WAV file format (tag: %d)", wf.formatTag));
    return false;
    break;
  }
  
  sampleRate = wf.sampleRate;
  
  return true;
}

//==============================================================================
bool SampleData::_ReadChunkData( const RiffChunkHeader& rch, SampleFormat sf,
  int hFile )
{
  if(sf == SF_UNKNOWN)
  {
    XR_ERROR(("Data sequence error in %s: format should preced data.", File::GetName(hFile)));
    return false;
  }

  //uint32  bufSize(bufferSize);
  //if(bufSize == 0 || rch.length == bufSize )
  //{
  //  XR_ERROR(("Unexpected data size in %s", File::GetName(hFile)));
  //  return false;
  //}

  int numBytes(rch.length);
  if(sf == SF_PCM_16_MONO)
  {
    numBytes /= 2;
  }
  _SetBufferSize(numBytes);
  
  switch(sf)
  {
  case  SF_ADPCM_MONO:
    if(File::Read(1, rch.length, hFile, parSamples) != rch.length)
    {
      XR_ERROR(("Error reading sample data from %s", File::GetName(hFile)));
      return false;
    }
    break;
    
  case  SF_PCM_16_MONO:
    {
      int     ns((rch.length / 2) & ~1);
      int16*  parBuffer(new int16[ns]);
      if(File::Read(2, ns, hFile, parBuffer) != (uint32)ns)
      {
        XR_ERROR(("Error reading sample data from %s", File::GetName(hFile)));
        return false;
      }
            
      int n(Adpcm::Encode(ns, parBuffer, parSamples));
      delete[] parBuffer;

      if(n != ns)
      {
        XR_ERROR(("Error encoding sample data from %s", File::GetName(hFile)));
        return false;
      }
      samplesPerBlock = 512;
      blockAlign = 256;
    }
    break;
  //case  SF_PCM_8_MONO:
  //  {
  //    int     ns(rch.length & ~1);
  //    uint8*  parBuffer(new uint8[ns]);
  //    if(File::Read(1, ns, hFile, parBuffer) != ns)
  //    {
  //      XR_ERROR(("Error reading sample data from %s", File::GetName(hFile)));
  //      return false;
  //    }
  //    
  //    int n(Adpcm::Encode(ns, parBuffer, parSamples));
  //    delete[] parBuffer;
  //    
  //    if(n != ns)
  //    {
  //      XR_ERROR(("Error encoding sample data from %s", File::GetName(hFile)));
  //      return false;
  //    }

  //    samplesPerBlock = 512;
  //    blockAlign = 256;
  //  }
  //  
  //  break;
  }
  
  return true;
}

//==============================================================================
bool SampleData::_ReadChunkFact( const RiffChunkHeader& rch, int hFile )
{
  uint32  n;
  if(File::Read(sizeof(n), 1, hFile, &n) != 1)
  {
    XR_ERROR(("Error reading wave file fact from %s", File::GetName(hFile)));
    return false;
  }
  
  numSamples = n;
  return true;
}

//==============================================================================
void SampleData::_SetBufferSize( int numBytes )
{
  if(parSamples == 0 || numBytes != bufferSize)
  {
    numSamples = numBytes & ~1;
    bufferSize = numBytes;

    delete[] parSamples;
    parSamples = new uint8[bufferSize];
  }
}

} // XR
