//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include <algorithm>
#include "xr/Tweener.hpp"
#include "xr/debug.hpp"

namespace xr
{

//==============================================================================
///@brief Updates the Param, decrementing its @a lifespan and updating the value
/// with the @a pFunction.
bool  Tweener::Param::Update(float tDelta)
{
  tDelta *= invDuration;
  bool  over(progress + tDelta >= 1.0f);
  if (over)
  {
    *value = vTarget;
    if (onFrame)
    {
      (*onFrame)(callbackData);
    }

    if (onFinished)
    {
      (*onFinished)(callbackData);
    }
  }
  else
  {
    progress += tDelta;
    *value = (*function)(vStart, vTarget, progress);
    if (onFrame)
    {
      (*onFrame)(callbackData);
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
///@brief Registers a Param for tweening.
void  Tweener::Add(float duration, Function function, float target,
        float& value, Callback onFrame, Callback onFinished, void* data)
{
  XR_ASSERT(Tweener, function != nullptr);
  XR_ASSERT(Tweener, duration >= .0f);
  if (duration > .0f)
  {
    Param  p = { .0f, 1.0f / duration, function, value, target, &value,
      onFrame, onFinished, data };

    ParamList::iterator iInsert(std::lower_bound(m_params.begin(),
      m_params.end(), p, Param::ComparePredicate()));
    if (iInsert != m_params.end() && iInsert->value == p.value)
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
    if (onFinished)
    {
      (*onFinished)(data);
    }
  }
}

//==============================================================================
///@brief Removes the Param for the given value (if found). @a finish indicates
/// whether the value should be set to its target and its onFinishedCb (if
/// any) be called.
///@return  Whether a value was removed.
bool  Tweener::Remove( float& value, bool finish )
{
  Param pComp = { .0f, .0f, 0, .0f, .0f, &value, nullptr, nullptr, nullptr };
  ParamList::iterator iRemove(std::lower_bound(m_params.begin(), m_params.end(),
    pComp, Param::ComparePredicate()));
  bool  success(iRemove != m_params.end() && iRemove->value == pComp.value);
  if (success)
  {
    if (finish)
    {
      value = iRemove->vTarget;
      Callback  onFinished(iRemove->onFinished);
      if (onFinished)
      {
        (*onFinished)(iRemove->callbackData);
      }
    }
    m_params.erase(iRemove);
  }
  return success;
}

//==============================================================================
///@brief Updates all Params, removing the ones that have expired.
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
///@brief Clears all Params.
void  Tweener::Clear()
{
  m_params.clear();
}

}  // xr
