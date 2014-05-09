//
// Nuclear Heart Games
// XRhodes
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#include  "ScreenManager.hpp"

namespace XR
{

//==============================================================================
ScreenManager::ScreenManager()
: m_pCurrent(0),
  m_pLast(0),
  m_container(),
  m_dispatcher(),
  m_renderer()
{}

//==============================================================================
ScreenManager::~ScreenManager()
{}

//==============================================================================
void  ScreenManager::Change(Screen* pScreen, uint32 delayMs)
{
  XR_ASSERT(ScreenManager, pScreen != 0);
  Screen* pPrevious(0);
  if(m_pCurrent != 0)
  {
    pPrevious = m_pCurrent->GetPrevious();
    m_pCurrent->Hide(delayMs);
    m_pCurrent->SetPrevious(0);
  }
  m_pLast = m_pCurrent;
  
  if(pScreen != 0)
  {
    pScreen->Show(*this, delayMs);
    pScreen->SetPrevious(pPrevious);
  }
  m_pCurrent = pScreen;
}

//==============================================================================
void  ScreenManager::Push(Screen* pScreen, uint32 delayMs)
{
  XR_ASSERT(ScreenManager, pScreen != 0);
  if(m_pCurrent != 0)
  {
    m_pCurrent->Unregister();
    pScreen->SetPrevious(m_pCurrent);
  }
  
  m_pLast = m_pCurrent;
  m_pCurrent = pScreen;
  pScreen->Show(*this, delayMs);
}

//==============================================================================vlkd  
void  ScreenManager::Pop(uint32 delayMs)
{
  XR_ASSERT(ScreenManager, m_pCurrent != 0);
  m_pLast = m_pCurrent;
  m_pCurrent->Hide(delayMs);
  
  Screen* pScreen(m_pCurrent->GetPrevious());
  m_pCurrent = pScreen;
  if(pScreen != 0)
  {
    pScreen->Register();
  }
}

//==============================================================================vlkd  
void  ScreenManager::Update(int32 ms)
{
  if(m_pCurrent != 0)
  {
    m_pCurrent->Update(ms);
  }
  
  if(m_pLast != 0)
  {
    m_pLast->Update(ms);
  }
}

//==============================================================================vlkd  
void  ScreenManager::Render()
{
  m_container.Render(&m_renderer);
}

} // XR
