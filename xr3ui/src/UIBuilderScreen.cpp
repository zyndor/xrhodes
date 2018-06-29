//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/UIBuilderScreen.hpp"
#include "xr/ScreenManager.hpp"
#include "xr/UIGrowingLayoutBase.hpp"

namespace xr
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

static const char* const kTags[kNumTags] =
{
  "listener",
  "tweening",
  "id"
};

//==============================================================================
const char* const UIBuilderScreen::kAnchorName[] =
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
  m_tweenIn(nullptr),
  m_tweenInData(nullptr),
  m_tweenOut(nullptr),
  m_tweenOutData(nullptr)
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
bool  UIBuilderScreen::Build(tinyxml2::XMLElement* xml)
{
#ifdef  XR_DEBUG
  tinyxml2::XMLDocument*  doc = xml->GetDocument();
  m_debugName = doc ? doc->Value() : "unknown";
#endif  //XR_DEBUG

  Destroy();

  XR_ASSERT(UIBuilderScreen, xml != nullptr);

  bool  result(m_builder.Build(xml, m_root));
  if(result)
  {
    _ProcessListeners(xml);
    _ProcessTweening(xml);

    Reposition(Gfx::GetLogicalWidth(), Gfx::GetLogicalHeight());
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
  Reposition(Gfx::GetLogicalWidth(), Gfx::GetLogicalHeight());
}

//==============================================================================
void  UIBuilderScreen::SetTweenIn(TweenCallback onTweenIn, void* data)
{
  m_tweenIn = onTweenIn;
  m_tweenInData = data;
}

//==============================================================================
void  UIBuilderScreen::SetTweenOut(TweenCallback onTweenOut, void* data)
{
  m_tweenOut= onTweenOut;
  m_tweenOutData = data;
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
  m_manager->GetContainer().AddElement(&m_root);
}

//==============================================================================
void  UIBuilderScreen::_Show(uint32_t ms)
{
  if (m_tweenIn && !m_tweening.empty())
  {
    float     percent = 1.0f / GetNumTweening();
    Tweenable t = { nullptr, 0, percent, ms };
    while (t.id < GetNumTweening())
    {
      t.elem = m_tweening[t.id];
      (*m_tweenIn)(t, m_tweenInData);
      t.percent += percent;
      ++t.id;
    }
  }
}

//==============================================================================
void  UIBuilderScreen::_Hide(uint32_t ms)
{
  if (m_tweenOut && !m_tweening.empty())
  {
    float     percent = 1.0f / GetNumTweening();
    Tweenable t = { nullptr, 0, percent, ms };
    while (t.id < GetNumTweening())
    {
      t.elem = m_tweening[t.id];
      (*m_tweenOut)(t, m_tweenOutData);
      t.percent += percent;
      ++t.id;
    }
  }
}

//==============================================================================
void  UIBuilderScreen::_RemoveElements()
{
  m_manager->GetContainer().RemoveElement(&m_root);
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
    if(auto elem = m_builder.GetElement(kAnchorName[i]))
    {
      int y(i / 3);
      int x(i - (y * 3));
      elem->Align(arX[x], arY[y],
        static_cast<UIElement::Alignment>(x + UIElement::AL_LOW),
        static_cast<UIElement::Alignment>(y + UIElement::AL_LOW));
    }
  }
}

//==============================================================================
void  UIBuilderScreen::_Register()
{
  UIEventNotifier&  dispatcher = m_manager->GetNotifier();
  for(auto l: m_listeners)
  {
    dispatcher.AddListener(l);
  }
}

//==============================================================================
void  UIBuilderScreen::_Unregister()
{
  UIEventNotifier&  dispatcher = m_manager->GetNotifier();
  for (auto l : m_listeners)
  {
    dispatcher.RemoveListener(l);
  }
}

//==============================================================================
void  UIBuilderScreen::_ProcessListeners(tinyxml2::XMLElement* xml)
{
  tinyxml2::XMLElement* listenerXml = xml->FirstChildElement(kTags[TAG_LISTENER]);
  UIElementList l;  // TODO: replace with vector, reserve, then std::move.
  while (listenerXml)
  {
    const char* handle = listenerXml->Attribute(kTags[TAG_ID]);
    XR_ASSERTMSG(UIBuilderScreen, handle != nullptr, ("'%s' is required in '%s'",
      kTags[TAG_ID], kTags[TAG_LISTENER]));

    UIElement*  elem = m_builder.GetElement(handle);
    XR_ASSERTMSG(UIBuilderScreen, elem != nullptr, ("the %s '%s' is undefined.",
      kTags[TAG_ID], handle));

    l.push_back(elem);

    listenerXml = listenerXml->NextSiblingElement(kTags[TAG_LISTENER]);
  }

  m_listeners.assign(l.begin(), l.end());
}

//==============================================================================
void  UIBuilderScreen::_ProcessTweening(tinyxml2::XMLElement* xml)
{
  tinyxml2::XMLElement* tweeningXml = xml->FirstChildElement(kTags[TAG_TWEENING]);
  UIElementList l;  // TODO: replace with vector, reserve, then std::move.
  while (tweeningXml)
  {
    const char* handle = tweeningXml->Attribute(kTags[TAG_ID]);
    XR_ASSERTMSG(UIBuilderScreen, handle != nullptr, ("'%s' is required in '%s'",
      kTags[TAG_ID], kTags[TAG_TWEENING]));

    UIElement*  elem = m_builder.GetElement(handle);
    XR_ASSERTMSG(UIBuilderScreen, elem != nullptr, ("the %s '%s' is undefined.",
      kTags[TAG_ID], handle));

    l.push_back(elem);

    tweeningXml = tweeningXml->NextSiblingElement(kTags[TAG_TWEENING]);
  }

  m_tweening.assign(l.begin(), l.end());
}

} // xr
