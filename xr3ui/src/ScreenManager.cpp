//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/ScreenManager.hpp"
#include "xr/debug.hpp"

namespace xr
{

//==============================================================================
ScreenManager::ScreenManager()
: m_stack(),
  m_previous(nullptr),
  m_container(),
  m_notifier()
{}

//==============================================================================
ScreenManager::~ScreenManager()
{
  XR_ASSERT(ScreenManager, m_stack.empty());
}

//==============================================================================
void  ScreenManager::Change(Screen& screen, int32_t delayMs)
{
  _ClearExiting();

  Screen* pPrevious = nullptr;
  if (!m_stack.empty())
  {
    pPrevious = m_stack.back();
    pPrevious->Hide(delayMs);
    m_stack.pop_back();
  }
  m_previous = pPrevious;

  m_stack.push_back(&screen);
  screen.Show(*this, delayMs);
}

//==============================================================================
void  ScreenManager::Push(Screen& screen, int32_t delayMs)
{
  Screen* pPrevious = nullptr;
  if (!m_stack.empty())
  {
    pPrevious = m_stack.back();
    pPrevious->Unregister();
  }
  //m_pPrevious = pPrevious;  // probably not necessary - we only care if it isn't on the stack

  m_stack.push_back(&screen);
  screen.Show(*this, delayMs);
}

//==============================================================================
void  ScreenManager::Pop(int32_t delayMs)
{
  XR_ASSERT(ScreenManager, !m_stack.empty());
  _ClearExiting();

  Screen* pCurrent = m_stack.back();
  m_previous = pCurrent;

  pCurrent->Hide(delayMs);
  m_stack.pop_back();

  if (!m_stack.empty())
  {
    Screen* screen = m_stack.back();
    if (screen)
    {
      screen->Register();
    }
  }
}

//==============================================================================
void  ScreenManager::Update(int32_t ms)
{
  if (!m_stack.empty())
  {
    m_stack.back()->Update(ms);
  }

  if (m_previous)
  {
    m_previous->Update(ms);
    if (m_previous->GetState() == Screen::State::Hidden)
    {
      m_previous = nullptr;
    }
  }
}

//==============================================================================
void  ScreenManager::Render(IUIRenderer& r)
{
  m_container.Render(r);
}

//==============================================================================
void  ScreenManager::Shutdown()
{
  _ClearExiting();

  while (!m_stack.empty())
  {
    Screen* pPrevious = m_stack.back();
    switch (pPrevious->GetState())
    {
    case Screen::State::Showing:
    case Screen::State::Active:
    case Screen::State::Hiding:
      pPrevious->Hide(0);
      break;

    default:
      break;
    }
    m_stack.pop_back();
  }
}

//==============================================================================
void  ScreenManager::_ClearExiting()
{
  if (m_previous)
  {
    switch (m_previous->GetState())
    {
    case Screen::State::Showing:
    case Screen::State::Active:
    case Screen::State::Hiding:
      m_previous->Hide(0);
      break;

    default:
      break;
    }
    m_previous = nullptr;
  }
}

} // xr
