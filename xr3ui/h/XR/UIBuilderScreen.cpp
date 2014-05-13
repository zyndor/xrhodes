//
// Nuclear Heart Games
// XRhodes
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#include "UIBuilderScreen.hpp"
#include "ScreenManager.hpp"
#include "UIGrowingLayoutBase.hpp"

namespace XR
{

//==============================================================================
typedef std::list<UIElement*> UIElementList;

enum
{
  TAG_LISTENER,
  TAG_TWEENING,
  TAG_ID,
  kNumTags
};

static const char* karTag[kNumTags] = 
{
  "listener",
  "tweening",
  "id"
};

//==============================================================================
const UIBuilderScreen::Configuration  UIBuilderScreen::kDefaultConfig =
{
  8,
  Sprite::Manager::Get,
  0,
  Font::Manager::Get,
  0,
  NewAllocate,
  NewDeallocate,
  0
};

const char* const UIBuilderScreen::karAnchorName[] =
{
  "top-left",
  "top",
  "top-right",
  "left",
  "center",
  "right",
  "bottom-left",
  "bottom",
  "bottom-right",
};

//==============================================================================
UIBuilderScreen::UIBuilderScreen ()
: Screen(),
  m_root(),
  m_builder(),
  m_numListeners(0),
  m_parpListeners(0),
  m_numTweening(0),
  m_parpTweening(0)
{}

//==============================================================================
UIBuilderScreen::~UIBuilderScreen ()
{
  Destroy();
}

//==============================================================================
bool  UIBuilderScreen::Build(TiXmlElement* pXml, const Configuration& cfg)
{
  Destroy();
  
  XR_ASSERT(UIBuilderScreen, pXml != 0);
  
  m_builder.RegisterGetSpriteCallback(cfg.pGetSprite, cfg.pGetSpriteData);
  m_builder.RegisterGetFontCallback(cfg.pGetFont, cfg.pGetFontData);
  
  bool  result(m_builder.Build(pXml, cfg.pAllocate, cfg.pDeallocate,
    cfg.pAllocateData, m_root));
  if(result)
  {
    _ProcessListeners(pXml, cfg);
    _ProcessTweening(pXml, cfg);

    m_pDeallocate = cfg.pDeallocate;
    m_pDeallocateData = cfg.pAllocateData;
    
    Reposition(Renderer::GetScreenWidth(), Renderer::GetScreenHeight());
  }
  return result;
}

//==============================================================================
void  UIBuilderScreen::Destroy()
{
  m_builder.Destroy();
  
  (*m_pDeallocate)(m_parpListeners, m_pDeallocateData);
  memset(m_parpListeners, sizeof(UIElement*) * m_numListeners, 0x00);
  m_numListeners = 0;
  
  (*m_pDeallocate)(m_parpTweening, m_pDeallocateData);
  memset(m_parpTweening, sizeof(UIElement*) * m_numTweening, 0x00);
  m_numTweening = 0;
}

//==============================================================================
void  UIBuilderScreen::SetPadding(int padding)
{
  m_padding = padding;
  Reposition(Renderer::GetScreenWidth(), Renderer::GetScreenHeight());
}

//==============================================================================
void  UIBuilderScreen::_AddElements()
{
  m_pManager->GetContainer().AddElement(&m_root);
}

//==============================================================================
void  UIBuilderScreen::_Show(uint32 ms)
{
  XR_ASSERT(UIBuilderScreen, m_pTweenIn != 0);
  if (m_numTweening > 0)
  {
    float     percent(1.0f / m_numTweening);
    Tweenable t = { 0, 0, percent, ms };
    while (t.id < m_numTweening)
    {
      t.pElem = m_parpTweening[t.id];
      (*m_pTweenIn)(t, m_pTweenInData);
      t.percent += percent;
      ++t.id;
    }
  }
}

//==============================================================================
void  UIBuilderScreen::_Hide(uint32 ms)
{
  XR_ASSERT(UIBuilderScreen, m_pTweenOut != 0);
  if (m_numTweening > 0)
  {
    float     percent(1.0f / m_numTweening);
    Tweenable t = { 0, 0, percent, ms };
    while (t.id < m_numTweening)
    {
      t.pElem = m_parpTweening[t.id];
      (*m_pTweenOut)(t, m_pTweenOutData);
      t.percent += percent;
      ++t.id;
    }
  }
}

//==============================================================================
void  UIBuilderScreen::_RemoveElements()
{
  m_pManager->GetContainer().RemoveElement(&m_root);
}

//==============================================================================
void  UIBuilderScreen::Reposition(int16 width, int16 height)
{
  XR_ASSERT(UIBuilderScreen, width > 0);
  XR_ASSERT(UIBuilderScreen, height > 0);
  const int16 spacing = UIGrowingLayoutBase::GetDefaultSpacing();
  const int16 arX[3] = { spacing, width / 2, width - spacing };
  const int16 arY[3] = { spacing, height / 2, height - spacing };
  for(int i = 0; i < kNumAnchors; ++i)
  {
    UIElement*  p(m_builder.GetElement(karTag[i]));
    if(p != 0)
    {
      int x(i % 3);
      int y(i / 3);
      p->Align(arX[x], arY[y],
        static_cast<UIElement::Alignment>(x + UIElement::AL_LOW),
        static_cast<UIElement::Alignment>(y + UIElement::AL_LOW));
    }
  }
}

//==============================================================================
void  UIBuilderScreen::_Register()
{
  UIEventDispatcher&  dispatcher(m_pManager->GetDispatcher());
  for (int i = 0; i < m_numListeners; ++i)
  {
    dispatcher.AddListener(m_parpListeners[i]);
  }
}

//==============================================================================
void  UIBuilderScreen::_Unregister()
{
  UIEventDispatcher&  dispatcher(m_pManager->GetDispatcher());
  for (int i = 0; i < m_numListeners; ++i)
  {
    dispatcher.RemoveListener(m_parpListeners[i]);
  }
}

//==============================================================================
void  UIBuilderScreen::_ProcessListeners(TiXmlElement* pXml,
        const Configuration& cfg)
{
  TiXmlElement* pListenerXml(pXml->FirstChildElement(karTag[TAG_LISTENER]));
  UIElementList l;
  while (pListenerXml != 0)
  {
    const char* pHandle(pListenerXml->Attribute(karTag[TAG_ID]));
    XR_ASSERTMSG(UIBuilderScreen, pHandle != 0, ("'%s' is required in '%s'",
      karTag[TAG_ID], karTag[TAG_LISTENER]));
    
    UIElement*  pElem(m_builder.GetElement(pHandle));
    XR_ASSERTMSG(UIBuilderScreen, pElem != 0, ("the %s '%s' is undefined.",
      karTag[TAG_ID], pHandle));
    
    l.push_back(pElem);
    
    pListenerXml = pListenerXml->NextSiblingElement(karTag[TAG_LISTENER]);
  }
  
  m_numListeners = l.size();
  void* pMem = static_cast<UIElement*>((*cfg.pAllocate)(sizeof(UIElement*) *
    m_numListeners, cfg.pAllocateData));
  m_parpListeners = new (pMem) UIElement*[m_numListeners];

  std::copy(l.begin(), l.end(), m_parpListeners);
}

//==============================================================================
void  UIBuilderScreen::_ProcessTweening(TiXmlElement* pXml,
        const Configuration& cfg)
{
  TiXmlElement* pTweeningXml(pXml->FirstChildElement(karTag[TAG_TWEENING]));
  UIElementList l;
  while (pTweeningXml != 0)
  {
    const char* pHandle(pTweeningXml->Attribute(karTag[TAG_ID]));
    XR_ASSERTMSG(UIBuilderScreen, pHandle != 0, ("'%s' is required in '%s'",
      karTag[TAG_ID], karTag[TAG_TWEENING]));
    
    UIElement*  pElem(m_builder.GetElement(pHandle));
    XR_ASSERTMSG(UIBuilderScreen, pElem != 0, ("the %s '%s' is undefined.",
      karTag[TAG_ID], pHandle));
    
    l.push_back(pElem);
    
    pTweeningXml = pTweeningXml->NextSiblingElement(karTag[TAG_TWEENING]);
  }
  
  m_numTweening = l.size();
  void* pMem = static_cast<UIElement*>((*cfg.pAllocate)(sizeof(UIElement*) *
    m_numTweening, cfg.pAllocateData));
  m_parpTweening = new (pMem) UIElement*[m_numListeners];
  
  std::copy(l.begin(), l.end(), m_parpTweening);
}

} // XR
