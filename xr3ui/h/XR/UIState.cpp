#include "UIState.hpp"

namespace XR
{

//==============================================================================
UIState* UIState::s_pActiveInstance = 0;

//==============================================================================
UIState::UIState()
: State(),
  m_modalities(),
  m_isModalityPopRequested(false),
  m_pRenderer(0)
{}

//==============================================================================
UIState::~UIState()
{}

//==============================================================================
void UIState::Init()
{
  s_pActiveInstance = this;
  
  m_isModalityPopRequested = false;

  PushModality(0, 0);
  
  _Init();
}

//==============================================================================
void UIState::Shutdown()
{
  _Shutdown();

  while (m_modalities.size() > 0)
  {
    PopModality();
  }
}

//==============================================================================
void UIState::Update(int32 ms)
{
  if (m_isModalityPopRequested)
  {
    PopModality();
    m_isModalityPopRequested = false;
  }

  _Update(ms);
}

//==============================================================================
void UIState::Render()
{
  _Render();

  //Renderer::LightingOn();

  int modalitiesRendered(0);
  if (m_pRenderer != 0)
  {
    for (ModalityList::iterator i0(m_modalities.begin()), i1(m_modalities.end());
      i0 != i1; ++i0)
    {
      _OnModalityRenderStart(modalitiesRendered);

      i0->container.Render(m_pRenderer);
      m_pRenderer->Render();
      
      ++modalitiesRendered;
    }
    
    m_pRenderer->Clear();
  }
  else
  {
    for (ModalityList::iterator i0(m_modalities.begin()), i1(m_modalities.end());
      i0 != i1; ++i0)
    {
      _OnModalityRenderStart(modalitiesRendered);

      i0->container.Render();

      ++modalitiesRendered;
    }
  }

  //Renderer::LightingOff();
}

//==============================================================================
void UIState::_Render()
{}

//==============================================================================
void UIState::PushModality(Callback pOnModalityPopCb, void* pOnModalityPopCbData)
{
  bool  result;
  if (m_modalities.size() > 0)
  {
    result = GetDispatcher().Unregister();
  }

  m_modalities.push_back(Modality());
  m_modalities.back().pOnModalityPopCb = pOnModalityPopCb;
  m_modalities.back().pOnModalityPopCbData = pOnModalityPopCbData;
  result = GetDispatcher().Register();
}

//==============================================================================
void UIState::PopModality()
{
  XR_ASSERTMSG(XR::UIState, m_modalities.size() > 0,
    ("No modalities to pop in UIState"));

  bool  result;
  result = GetDispatcher().Unregister();
  
  if (m_modalities.back().pOnModalityPopCb != 0)
  {
    (*m_modalities.back().pOnModalityPopCb)(m_modalities.back().pOnModalityPopCbData);
  }
  m_modalities.pop_back();
  
  if (m_modalities.size() > 0)
  {
    result = GetDispatcher().Register();
  }
}

//==============================================================================
void UIState::_OnModalityRenderStart( int modalitiesRendered )
{}

//==============================================================================
void UIState::RequestModalityPop()
{
  XR_ASSERTMSG(XR::UIState, m_modalities.size() > 1,
    ("Too many modality pops requested in UIState."));

  m_isModalityPopRequested = true;
}

//==============================================================================
void  UIState::SetRenderer(UIRenderer* pRenderer)
{
  m_pRenderer = pRenderer;
}

} // XR
