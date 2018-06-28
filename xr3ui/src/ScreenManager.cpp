//
// Nuclear Heart Games
// XRhodes
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#include "xr/ScreenManager.hpp"

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
{}

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
    if (m_previous->GetState() == Screen::S_HIDDEN)
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
    if (pPrevious->GetState() <= Screen::S_HIDING)
    {
      pPrevious->Hide(0);
    }
    m_stack.pop_back();
  }
}

//==============================================================================
void  ScreenManager::_ClearExiting()
{
  if (m_previous)
  {
    if (m_previous->GetState() <= Screen::S_HIDING)
    {
      m_previous->Hide(0);
    }
    m_previous = nullptr;
  }
}

} // xr
