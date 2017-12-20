//
// Nuclear Heart Games
// XRhodes
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#include "XR/ScreenManager.hpp"

namespace XR
{

//==============================================================================
ScreenManager::ScreenManager()
: m_stack(),
  m_pPrevious(0),
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

  Screen* pPrevious(0);
  if (!m_stack.empty())
  {
    pPrevious = m_stack.back();
    pPrevious->Hide(delayMs);
    m_stack.pop_back();
  }
  m_pPrevious = pPrevious;

  m_stack.push_back(&screen);
  screen.Show(*this, delayMs);
}

//==============================================================================
void  ScreenManager::Push(Screen& screen, int32_t delayMs)
{
  Screen* pPrevious(0);
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

  Screen* pCurrent(m_stack.back());
  m_pPrevious = pCurrent;

  pCurrent->Hide(delayMs);
  m_stack.pop_back();

  if (!m_stack.empty())
  {
    Screen* pScreen(m_stack.back());
    if (pScreen != 0)
    {
      pScreen->Register();
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

  if (m_pPrevious != 0)
  {
    m_pPrevious->Update(ms);
    if (m_pPrevious->GetState() == Screen::S_HIDDEN)
    {
      m_pPrevious = 0;
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
    Screen* pPrevious(m_stack.back());
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
  if (m_pPrevious != 0)
  {
    if (m_pPrevious->GetState() <= Screen::S_HIDING)
    {
      m_pPrevious->Hide(0);
    }
    m_pPrevious = 0;
  }
}

} // XR
