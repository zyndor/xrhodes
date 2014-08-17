//
// Nuclear Heart Games
// XRhodes
//
// UIBuilderScreen.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    09/05/2014
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#ifndef XR_UIBUILDERSCREEN_HPP
#define XR_UIBUILDERSCREEN_HPP

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
    int         id;
    float       percent;
    uint32      delayMs;
  };
  
  typedef void(*TweenCallback)(Tweenable& t, void* pUser);
  
  // static
  static const char* const    karAnchorName[kNumAnchors];
  
  // structors
  explicit UIBuilderScreen(const UIBuilder::Configuration& cfg = UIBuilder::kDefaultConfig);
  ~UIBuilderScreen();
  
  // general
  virtual void        Reposition(int16 width, int16 height);
  
  void                SetConfiguration(const UIBuilder::Configuration& cfg);
  
  bool                Build(TiXmlElement* pXml);
  UIElement*          GetElement(const char* pName);
  UIElement*          GetElement(uint32 hash);
  void                Destroy();
  
  int                 GetPadding() const;
  void                SetPadding(int padding);
  
  void                SetTweenIn(TweenCallback pOnTweenIn, void* pData);
  void                SetTweenOut(TweenCallback pOnTweenIn, void* pData);
  
  int                 GetNumListeners() const;
  UIElement* const *  GetListeners() const; // no ownership transfer
  int                 GetNumTweening() const;
  UIElement* const *  GetTweening() const; // no ownership transfer
  
protected:
  // data
  UIContainer                   m_root;
  UIBuilder                     m_builder;
  int                           m_padding;
  
  int                           m_numListeners;
  UIElement**                   m_parpListeners;  // own the array, not the elements
  
  int                           m_numTweening;
  UIElement**                   m_parpTweening;  // own the array, not the elements
  
  TweenCallback                 m_pTweenIn;
  void*                         m_pTweenInData;
  
  TweenCallback                 m_pTweenOut;
  void*                         m_pTweenOutData;
  
  // internal
  virtual void  _AddElements();
  virtual void  _Show(uint32 ms);
  virtual void  _Hide(uint32 ms);
  virtual void  _RemoveElements();
  
  virtual void  _Register();
  virtual void  _Unregister();
  
  void          _ProcessListeners(TiXmlElement* pXml);
  void          _ProcessTweening(TiXmlElement* pXml);
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
UIElement*  UIBuilderScreen::GetElement(uint32 hash)
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
int UIBuilderScreen::GetNumListeners() const
{
  return m_numListeners;
}

//==============================================================================
inline
UIElement* const *  UIBuilderScreen::GetListeners() const
{
  return m_parpListeners;
}

//==============================================================================
inline
int UIBuilderScreen::GetNumTweening() const
{
  return m_numTweening;
}

//==============================================================================
inline
UIElement* const *  UIBuilderScreen::GetTweening() const
{
  return m_parpTweening;
}

} // XR

#endif // XR_UIBUILDERSCREEN_HPP
