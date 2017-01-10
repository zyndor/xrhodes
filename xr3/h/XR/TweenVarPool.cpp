//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "TweenVarPool.hpp"
#include "debug.hpp"

namespace XR
{

TweenVarPoolCore::TweenVarPoolCore( int numVars )
: m_numVars(numVars),
  m_parVarBuffer(0)
{
  XR_ASSERT(TweenVarPoolCore, numVars >= 0);
  m_parVarBuffer = new VarCore[numVars];

  for (int i = 0; i < numVars; ++i)
  {
    m_parVarBuffer[i].Clear();
    m_parVarBuffer[i].offset = i;
  }
}

TweenVarPoolCore::~TweenVarPoolCore()
{
  for (int i = 0; i < m_numVars; ++i)
  {
    if (m_parVarBuffer[i].pTweener != 0)
    {
      m_parVarBuffer[i].pTweener->Remove(m_parVarBuffer[i].fValue, false);
    }
  }
  delete[] m_parVarBuffer;
}

} // XR
