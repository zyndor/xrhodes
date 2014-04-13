#include "Sample.hpp"
#include "File.hpp"
#include "SampleData.hpp"

#define TO_IMPL() (static_cast<SampleData*>(m_pImpl))
#define TO_CONST_IMPL() (static_cast<const SampleData*>(m_pImpl))

namespace XR
{

XR_CROSS_OBJECT_DEF(Sample, SampleData)

int Sample::GetNumSamples() const
{
  return TO_CONST_IMPL()->numSamples;
}

void* Sample::GetData() const
{
  return TO_CONST_IMPL()->parSamples;
}

bool Sample::Load( const char* pName )
{
  return TO_IMPL()->Load(pName);
}

} // XR
