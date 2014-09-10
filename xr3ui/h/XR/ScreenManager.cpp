//
// Nuclear Heart Games
// XRhodes
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#include "ScreenManager.hpp"

namespace XR
{

//==============================================================================
ScreenManager::ScreenManager()
: m_pCurrent(0),
  m_pPrevious(0),
  m_container(),
  m_dispatcher(),
  m_renderer()
{}

//==============================================================================
ScreenManager::~ScreenManager()
{}

//==============================================================================
void  ScreenManager::Init(int numSprites)
{
  XR_ASSERT(ScreenManager, numSprites >= 0);
  m_renderer.Init(numSprites);
}

//==============================================================================
void  ScreenManager::Change(Screen* pScreen, uint32 delayMs)
{
  XR_ASSERT(ScreenManager, pScreen != 0);
  Screen* pPrevious(0);
  if (m_pCurrent != 0)
  {
    pPrevious = m_pCurrent->GetPrevious();
    m_pCurrent->Hide(delayMs);
    m_pCurrent->SetPrevious(0);
  }
  m_pPrevious = m_pCurrent;
  
  if (pScreen != 0)
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
  if (m_pCurrent != 0)
  {
    m_pCurrent->Unregister();
    pScreen->SetPrevious(m_pCurrent);
  }
  
  m_pPrevious = m_pCurrent;
  m_pCurrent = pScreen;
  pScreen->Show(*this, delayMs);
}

//==============================================================================
void  ScreenManager::Pop(uint32 delayMs)
{
  XR_ASSERT(ScreenManager, m_pCurrent != 0);
  m_pPrevious = m_pCurrent;
  m_pCurrent->Hide(delayMs);
  
  Screen* pScreen(m_pCurrent->GetPrevious());
  m_pCurrent = pScreen;
  if (pScreen != 0)
  {
    pScreen->Register();
  }
}

//==============================================================================
void  ScreenManager::Update(int32 ms)
{
  if (m_pCurrent != 0)
  {
    m_pCurrent->Update(ms);
  }
  
  if (m_pPrevious != 0)
  {
    m_pPrevious->Update(ms);
  }
}

//==============================================================================
void  ScreenManager::Render()
{
  //Renderer::Flush();
  
  Renderer::ClearBuffer(XR::Renderer::BF_DEPTH);
  Renderer::SetOrtho(.0f, (float)Renderer::GetScreenWidth(),
    (float)Renderer::GetScreenHeight(), .0f, 1000.0f, -1000.0f);
  Renderer::SetViewMatrix(Matrix::s_identity);
  Renderer::SetModelMatrix(Matrix::s_identity);
  
  m_renderer.Clear();
  m_container.Render(&m_renderer);
  m_renderer.Render();

  Renderer::Flush();
}

//==============================================================================
void  ScreenManager::Shutdown()
{
  if(m_pPrevious != 0 && m_pPrevious->GetState() <= Screen::S_HIDING)
  {
    m_pPrevious->Hide(0);
  }

  while(m_pCurrent != 0)
  {
    Screen* pPrevious(m_pCurrent->GetPrevious());
    if(m_pCurrent->GetState() <= Screen::S_HIDING)
    {
      m_pCurrent->Hide(0);
    }
    m_pCurrent = pPrevious;
  }

  m_renderer.Shutdown();
}

} // XR
