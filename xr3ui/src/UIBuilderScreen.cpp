//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/UIBuilderScreen.hpp"
#include "XR/ScreenManager.hpp"
#include "XR/UIGrowingLayoutBase.hpp"

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

static const char* const karTag[kNumTags] = 
{
  "listener",
  "tweening",
  "id"
};

//==============================================================================
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
UIBuilderScreen::UIBuilderScreen(const UIBuilder::Configuration& cfg)
: Screen(),
  m_root(),
  m_builder(),
  m_padding(0),
  m_pTweenIn(0),
  m_pTweenInData(0),
  m_pTweenOut(0),
  m_pTweenOutData(0)
{}

//==============================================================================
UIBuilderScreen::~UIBuilderScreen ()
{
  Destroy();
}

//==============================================================================
void  UIBuilderScreen::SetConfiguration(const UIBuilder::Configuration& cfg)
{
  m_builder.SetConfiguration(cfg);
}

//==============================================================================
bool  UIBuilderScreen::Build(TiXmlElement* pXml)
{
#ifdef  XR_DEBUG
  TiXmlDocument*  pDoc = pXml->GetDocument();
  m_debugName = pDoc ? pDoc->Value() : "unknown";
#endif  //XR_DEBUG

  Destroy();
  
  XR_ASSERT(UIBuilderScreen, pXml != 0);
  
  bool  result(m_builder.Build(pXml, m_root));
  if(result)
  {
    _ProcessListeners(pXml);
    _ProcessTweening(pXml);

    Reposition(Renderer::GetScreenWidth(), Renderer::GetScreenHeight());
  }
  return result;
}

//==============================================================================
void  UIBuilderScreen::Destroy()
{
  m_builder.Destroy();

  UIElementVector().swap(m_listeners);
  UIElementVector().swap(m_tweening);
}

//==============================================================================
void  UIBuilderScreen::SetPadding(int padding)
{
  XR_ASSERT(UIBuilderScreen, padding >= 0);
  m_padding = padding;
  Reposition(Renderer::GetScreenWidth(), Renderer::GetScreenHeight());
}

//==============================================================================
void  UIBuilderScreen::SetTweenIn(TweenCallback pOnTweenIn, void* pData)
{
  m_pTweenIn = pOnTweenIn;
  m_pTweenInData = pData;
}

//==============================================================================
void  UIBuilderScreen::SetTweenOut(TweenCallback pOnTweenOut, void* pData)
{
  m_pTweenOut= pOnTweenOut;
  m_pTweenOutData = pData;
}

//==============================================================================
void  UIBuilderScreen::MoveTweening(int x, int y)
{
  for(auto t: m_tweening)
  {
    t->Move(x, y);
  }
}

//==============================================================================
void  UIBuilderScreen::_AddElements()
{
  m_pManager->GetContainer().AddElement(&m_root);
}

//==============================================================================
void  UIBuilderScreen::_Show(uint32_t ms)
{
  if (m_pTweenIn != 0 && !m_tweening.empty())
  {
    float     percent(1.0f / GetNumTweening());
    Tweenable t = { 0, 0, percent, ms };
    while (t.id < GetNumTweening())
    {
      t.pElem = m_tweening[t.id];
      (*m_pTweenIn)(t, m_pTweenInData);
      t.percent += percent;
      ++t.id;
    }
  }
}

//==============================================================================
void  UIBuilderScreen::_Hide(uint32_t ms)
{
  if (m_pTweenOut != 0 && !m_tweening.empty())
  {
    float     percent(1.0f / GetNumTweening());
    Tweenable t = { 0, 0, percent, ms };
    while (t.id < GetNumTweening())
    {
      t.pElem = m_tweening[t.id];
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
void  UIBuilderScreen::Reposition(int width, int height)
{
  XR_ASSERT(UIBuilderScreen, width > 0);
  XR_ASSERT(UIBuilderScreen, height > 0);
  const int arX[3] = { m_padding, width / 2, width - m_padding };
  const int arY[3] = { m_padding, height / 2, height - m_padding };
  for(int i = 0; i < kNumAnchors; ++i)
  {
    UIElement*  p(m_builder.GetElement(karAnchorName[i]));
    if(p != 0)
    {
      int y(i / 3);
      int x(i - (y * 3));
      p->Align(arX[x], arY[y],
        static_cast<UIElement::Alignment>(x + UIElement::AL_LOW),
        static_cast<UIElement::Alignment>(y + UIElement::AL_LOW));
    }
  }
}

//==============================================================================
void  UIBuilderScreen::_Register()
{
  UIEventNotifier&  dispatcher(m_pManager->GetNotifier());
  for(auto l: m_listeners)
  {
    dispatcher.AddListener(l);
  }
}

//==============================================================================
void  UIBuilderScreen::_Unregister()
{
  UIEventNotifier&  dispatcher(m_pManager->GetNotifier());
  for (auto l : m_listeners)
  {
    dispatcher.RemoveListener(l);
  }
}

//==============================================================================
void  UIBuilderScreen::_ProcessListeners(TiXmlElement* pXml)
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
  
  m_listeners.assign(l.begin(), l.end());
}

//==============================================================================
void  UIBuilderScreen::_ProcessTweening(TiXmlElement* pXml)
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
  
  m_tweening.assign(l.begin(), l.end());
}

} // XR
