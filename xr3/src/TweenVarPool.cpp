//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/TweenVarPool.hpp"
#include "XR/debug.hpp"

namespace XR
{

TweenVarPoolCore::TweenVarPoolCore( int numVars )
: m_numVars(numVars),
  m_varPool(nullptr)
{
  XR_ASSERT(TweenVarPoolCore, numVars >= 0);
  m_varPool = new VarCore[numVars];

  for (int i = 0; i < numVars; ++i)
  {
    m_varPool[i].Clear();
    m_varPool[i].offset = i;
  }
}

TweenVarPoolCore::~TweenVarPoolCore()
{
  for (int i = 0; i < m_numVars; ++i)
  {
    if (m_varPool[i].tweener)
    {
      m_varPool[i].tweener->Remove(m_varPool[i].fValue, false);
    }
  }
  delete[] m_varPool;
}

} // XR
