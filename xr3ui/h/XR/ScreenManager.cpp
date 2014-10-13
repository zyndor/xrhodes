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
: m_stack(),
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
void  ScreenManager::Change(Screen* pScreen, int32 delayMs)
{
  XR_ASSERT(ScreenManager, pScreen != 0);
  Screen* pPrevious(0);
  if (!m_stack.empty())
  {
    pPrevious = m_stack.back();
    pPrevious->Hide(delayMs);
    m_stack.pop_back();
  }
  m_pPrevious = pPrevious;
  
  if (pScreen != 0)
  {
    pScreen->Show(*this, delayMs);
    m_stack.push_back(pScreen);
  }
}

//==============================================================================
void  ScreenManager::Push(Screen* pScreen, int32 delayMs)
{
  XR_ASSERT(ScreenManager, pScreen != 0);
  Screen* pPrevious(0);
  if (!m_stack.empty())
  {
    pPrevious = m_stack.back();
    pPrevious->Unregister();
  }
  
  m_pPrevious = pPrevious;
  m_stack.push_back(pScreen);
  pScreen->Show(*this, delayMs);
}

//==============================================================================
void  ScreenManager::Pop(int32 delayMs)
{
  XR_ASSERT(ScreenManager, !m_stack.empty());
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
void  ScreenManager::Update(int32 ms)
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

  while(!m_stack.empty())
  {
    Screen* pPrevious(m_stack.back());
    if(pPrevious->GetState() <= Screen::S_HIDING)
    {
      pPrevious->Hide(0);
    }
    m_stack.pop_back();
  }

  m_renderer.Shutdown();
}

} // XR
