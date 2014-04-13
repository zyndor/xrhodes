//
// Nuclear Heart Games
// XRhodes
//
// tweener.cpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date  12/07/2011
//
//==============================================================================
#include <algorithm>
#include "Tweener.hpp"

namespace XR
{

//==============================================================================
///@brief Updates the Param, decrementing its @a lifespan and updating the value
/// with the @a function.
bool  Tweener::Param::Update(float tDelta)
{
  bool  over(tDelta >= lifespan);
  if (over)
  {
    lifespan = .0f;
    *pValue = target;
    if (pOnFrame != 0)
    {
      (*pOnFrame)(pCallbackData);
    }

    if (pOnFinished != 0)
    {
      (*pOnFinished)(pCallbackData);
    }
  }
  else
  {
    lifespan -= tDelta;
    *pValue = (*pFunction)(lifespan, duration, target, delta);
    if (pOnFrame != 0)
    {
      (*pOnFrame)(pCallbackData);
    }
  }
  return over;
}

//==============================================================================
Tweener::Tweener()
: m_params()
{}

//==============================================================================
Tweener::~Tweener()
{}

//==============================================================================
///@brief Registers a param for tweening.
void  Tweener::Add(float duration, Function pFunction, float target,
  float& value, Callback pOnFrame, Callback pOnFinished, void* pData)
{
  XR_ASSERT(RingBuffer, pFunction != 0);
  XR_ASSERT(RingBuffer, duration >= .0f);
  if (duration > .0f)
  {
    Param  p = { duration, 1.0f / duration, pFunction, target,
      value - target, &value, pOnFrame, pOnFinished, pData };

    ParamList::iterator iInsert(std::lower_bound(m_params.begin(),
      m_params.end(), p, Param::ComparePredicate()));

    if(iInsert != m_params.end() && iInsert->pValue == p.pValue)
    {
      *iInsert = p;
    }
    else
    {
      m_params.insert(iInsert, p);
    }
  }
  else
  {
    value = target;
    if (pOnFinished != 0)
    {
      (*pOnFinished)(pData);
    }
  }
}

//==============================================================================
///@brief Removes the param for the given value (if found). @a finish indicates
/// whether the value should be set to its target and its pOnFinishedCb (if
/// any) be called.
///@return  Whether a value was removed.
bool  Tweener::Remove( float& value, bool finish )
{
  Param pComp = { .0f, .0f, 0, .0f, .0f, &value, 0, 0, 0 };
  ParamList::iterator iRemove(std::lower_bound(m_params.begin(), m_params.end(), 
    pComp, Param::ComparePredicate()));
  bool  success(iRemove != m_params.end() && iRemove->pValue == pComp.pValue);
  if(success)
  {
    if(finish)
    { 
      value = iRemove->target;
      Callback  pOnFinished(iRemove->pOnFinished);
      if(pOnFinished != 0)
      {
        (*pOnFinished)(iRemove->pCallbackData);
      }
    }
    m_params.erase(iRemove);
  }
  return success;
}

//==============================================================================
///@brief Updates all params, removing the ones that have expired.
void  Tweener::Update(float tDelta)
{
  ParamList::iterator i0(m_params.begin());
  ParamList::iterator i1(m_params.end());
  while (i0 != i1)
  {
    Param  &p(*i0);
    if (p.Update(tDelta))
    {
      i0 = m_params.erase(i0);
    }
    else
    {
      ++i0;
    }
  }
}

//==============================================================================
///@brief Clears all params.
void  Tweener::Clear()
{
  m_params.clear();
}

}  // XR
