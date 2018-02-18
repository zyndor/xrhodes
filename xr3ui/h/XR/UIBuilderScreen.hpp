#ifndef XR_UIBUILDERSCREEN_HPP
#define XR_UIBUILDERSCREEN_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include "Screen.hpp"
#include "UIBuilder.hpp"

namespace XR
{

//==============================================================================
class UIBuilderScreen:  public Screen
{
public:
  // types
  enum  Anchor
  {
    A_TOP_LEFT,
    A_TOP,
    A_TOP_RIGHT,
    A_LEFT,
    A_CENTER,
    A_RIGHT,
    A_BOTTOM_LEFT,
    A_BOTTOM,
    A_BOTTOM_RIGHT,
    kNumAnchors
  };

  struct  Tweenable
  {
    UIElement*  pElem;
    size_t      id;
    float       percent;
    uint32_t    delayMs;
  };

  typedef void(*TweenCallback)(Tweenable& t, void* pUser);

  typedef std::vector<UIElement*> UIElementVector;

  // static
  static const char* const    karAnchorName[kNumAnchors];

  // structors
  explicit UIBuilderScreen(const UIBuilder::Configuration& cfg = UIBuilder::kDefaultConfig);
  ~UIBuilderScreen();

  // general
  virtual void        Reposition(int width, int height);

  void                SetConfiguration(const UIBuilder::Configuration& cfg);

  bool                Build(tinyxml2::XMLElement* pXml);
  UIElement*          GetElement(const char* pName);
  UIElement*          GetElement(uint32_t hash);

  int                 GetPadding() const;
  void                SetPadding(int padding);

  void                SetTweenIn(TweenCallback pOnTweenIn, void* pData);
  void                SetTweenOut(TweenCallback pOnTweenIn, void* pData);

  size_t              GetNumListeners() const;
  UIElementVector const&  GetListeners() const; // no ownership transfer
  size_t              GetNumTweening() const;
  UIElementVector const&  GetTweening() const; // no ownership transfer

  void                MoveTweening(int x, int y);

  void                Destroy();

protected:
  // data
#ifdef  XR_DEBUG
  std::string   m_debugName;
#endif  //XR_DEBUG

  UIContainer   m_root;
  UIBuilder     m_builder;
  int           m_padding;

  std::vector<UIElement*> m_listeners;
  std::vector<UIElement*> m_tweening;

  TweenCallback m_pTweenIn;
  void*         m_pTweenInData;

  TweenCallback m_pTweenOut;
  void*         m_pTweenOutData;

  // internal
  virtual void  _AddElements();
  virtual void  _Show(uint32_t ms);
  virtual void  _Hide(uint32_t ms);
  virtual void  _RemoveElements();

  virtual void  _Register();
  virtual void  _Unregister();

  void          _ProcessListeners(tinyxml2::XMLElement* pXml);
  void          _ProcessTweening(tinyxml2::XMLElement* pXml);
};

//==============================================================================
// implementation
//==============================================================================
inline
UIElement*  UIBuilderScreen::GetElement(const char* pHandle)
{
  return m_builder.GetElement(pHandle);
}

//==============================================================================
inline
UIElement*  UIBuilderScreen::GetElement(uint32_t hash)
{
  return m_builder.GetElement(hash);
}

//==============================================================================
inline
int UIBuilderScreen::GetPadding() const
{
  return m_padding;
}

//==============================================================================
inline
size_t UIBuilderScreen::GetNumListeners() const
{
  return m_listeners.size();
}

//==============================================================================
inline
UIBuilderScreen::UIElementVector const& UIBuilderScreen::GetListeners() const
{
  return m_listeners;
}

//==============================================================================
inline
size_t UIBuilderScreen::GetNumTweening() const
{
  return m_tweening.size();
}

//==============================================================================
inline
UIBuilderScreen::UIElementVector const& UIBuilderScreen::GetTweening() const
{
  return m_tweening;
}

} // XR

#endif // XR_UIBUILDERSCREEN_HPP
