//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Signal.hpp"
#include "xr/debug.hpp"
#include <algorithm>

namespace xr
{

//==============================================================================
bool SignalCore::ConnectImpl(CallbackBase const& cb)
{
  bool result = !Find(cb, nullptr);
  if (result)
  {
    if (m_isDispatching) // deferred add
    {
      m_toConnect.emplace_back(cb.Clone());
    }
    else
    {
      m_callbacks.emplace_back(cb.Clone());
    }
  }
  return result;
}

//==============================================================================
bool SignalCore::DisconnectImpl(CallbackBase const& cb)
{
  CallbackVector::iterator iFind;
  const bool result = Find(cb, &iFind);
  if (result)
  {
    if (m_isDispatching) // deferred remove
    {
      m_toDisconnect.emplace_back(iFind->get());
    }
    else
    {
      m_callbacks.erase(iFind);
    }
  }
  return result;
}

//==============================================================================
std::vector<SignalCore::CallbackPtr>& SignalCore::GetCallbacks()
{
  return m_callbacks;
}


//==============================================================================
void SignalCore::SetDispatching(bool state)
{
  XR_ASSERT(SignalCore, state != m_isDispatching);
  if (!state)
  {
    FinishDispatching();
  }

  m_isDispatching = state;
}

//==============================================================================
bool SignalCore::Find(CallbackBase const& cb, CallbackVector::iterator* iRemove)
{
  auto iFind = std::find_if(m_callbacks.rbegin(), m_callbacks.rend(),
    [&cb](CallbackPtr const& cb2) {
      return cb2->operator==(cb);
    });
  bool result = iFind != m_callbacks.rend();
  if (result && iRemove)
  {
    *iRemove = --(iFind.base());
  }

  if (result) // currently added - do we have a deferred remove?
  {
    auto iFind2 = std::find_if(m_toDisconnect.rbegin(), m_toDisconnect.rend(),
      [&cb](CallbackBase* cb2) {
        return cb.operator==(*cb2);
      });
    if (iFind2 != m_toDisconnect.rend())
    {
      result = false;
      if (!iRemove)
      {
        // if found a pending remove while adding, cancel it.
        m_toDisconnect.erase(--(iFind2.base()));
      }
    }
  }
  else // not added -- do we have a deferred add?
  {
    auto iFind2 = std::find_if(m_toConnect.rbegin(), m_toConnect.rend(),
      [&cb](CallbackPtr const& cb2) {
        return cb.operator==(*cb2);
      });
    if (iFind2 != m_toConnect.rend())
    {
      result = true;
      if (iRemove)
      {
        // if found a pending add while removing, cancel it.
        m_toConnect.erase(--(iFind2.base()));
      }
    }
  }
  return result;
}

//==============================================================================
void SignalCore::FinishDispatching()
{
  auto iRemove = m_callbacks.end();
  for (auto& i: m_toDisconnect)
  {
    iRemove = std::remove_if(m_callbacks.begin(), iRemove,
      [&i](CallbackPtr const& cb){
        return cb.get() == i;
      });
  }
  m_callbacks.erase(iRemove, m_callbacks.end());
  m_toDisconnect.clear();

  for (auto& i: m_toConnect)
  {
    m_callbacks.emplace_back(std::move(i));
  }
  m_toConnect.clear();

  m_isDispatching = false;
}

}
