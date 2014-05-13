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
  
  struct  Configuration
  {
    int                           depth;
    Sprite::GetCallback           pGetSprite;
    void*                         pGetSpriteData;
    Font::GetCallback             pGetFont;
    void*                         pGetFontData;
    UIBuilder::AllocateCallback   pAllocate;
    UIBuilder::DeallocateCallback pDeallocate;
    void*                         pAllocateData;
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
  static const Configuration  kDefaultConfig;
  static const char* const    karAnchorName[kNumAnchors];
  
  // structors
  UIBuilderScreen();
  ~UIBuilderScreen();
  
  // general
  virtual void        Reposition(int16 width, int16 height);
  
  bool                Build(TiXmlElement* pXml,
                        const Configuration& cfg = kDefaultConfig);
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
  
  UIBuilder::DeallocateCallback m_pDeallocate;
  void*                         m_pDeallocateData;
  
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
  
  void          _ProcessListeners(TiXmlElement* pXml, const Configuration& cfg);
  void          _ProcessTweening(TiXmlElement* pXml, const Configuration& cfg);
};

//==============================================================================
// implementation
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
