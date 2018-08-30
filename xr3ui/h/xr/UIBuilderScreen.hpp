#ifndef XR_UIBUILDERSCREEN_HPP
#define XR_UIBUILDERSCREEN_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "Screen.hpp"
#include "UIBuilder.hpp"

namespace xr
{

//==============================================================================
///@brief Offers capabilities to build a screen from a UIBuilder xml document,
/// and adds bridging functionality for integrating with ScreenManager.
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
    kCount
  };

  struct  Tweenable
  {
    UIElement*  elem;
    size_t      id;
    float       percent;
    uint32_t    delayMs;
  };

  using TweenCallback = void(*)(Tweenable& t, void* userData);

  using UIElementVector = std::vector<UIElement*>;

  // static
  static const char* const  kAnchorName[kCount];

  // structors
  explicit UIBuilderScreen(const UIBuilder::Configuration& cfg = UIBuilder::kDefaultConfig);
  ~UIBuilderScreen();

  // general
  virtual void  Reposition(int width, int height);

  ///@brief Updates its UIBuilder configuration with @a cfg.
  void  SetConfiguration(const UIBuilder::Configuration& cfg);

  ///@brief Attempts to build the Screen from the given @a xml.
  bool  Build(tinyxml2::XMLElement* xml);

  ///@return UIElement with the given handle; nullptr if wasn't found.
  UIElement*  GetElement(const char* handle);

  ///@return UIElement with the given hash of a handle; nullptr if wasn't found.
  UIElement*  GetElement(uint32_t hash);

  ///@brief Sets the amount of padding around anchor elements.
  void  SetPadding(int padding);

  ///@return The amount of padding applied around anchor elements.
  int GetPadding() const;

  ///@brief Sets the function to handle the showing animation.
  void  SetTweenIn(TweenCallback onTweenIn, void* userData);

  ///@brief Sets the function to handle the hiding animation.
  void  SetTweenOut(TweenCallback onTweenIn, void* userData);

  ///@return The number of elements intended for mouse event listeners.
  size_t GetNumListeners() const;

  ///@return The vector of elements intended moust event listeners.
  UIElementVector const&  GetListeners() const; // no ownership transfer

  ///@return The number of elements intended for showing / hiding animations.
  size_t GetNumTweening() const;

  ///@return The vector of elements intended for showing / hiding animations.
  UIElementVector const&  GetTweening() const; // no ownership transfer

  ///@brief Helper function to offset the position of tweening registered
  /// elements, i.e. prior to an animation.
  void  MoveTweening(int x, int y);

  ///@brief Destroys the elements and clears the list of tweening and listeners.
  ///@note The screen must be hidden (and unregistered).
  void  Destroy();

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

  TweenCallback m_tweenIn;
  void*         m_tweenInData;

  TweenCallback m_tweenOut;
  void*         m_tweenOutData;

  // internal
  virtual void  AddElements();
  virtual void  ShowImpl(uint32_t ms);
  virtual void  HideImpl(uint32_t ms);
  virtual void  RemoveElements();

  virtual void  RegisterImpl();
  virtual void  UnregisterImpl();

  void          ProcessListeners(tinyxml2::XMLElement* xml);
  void          ProcessTweening(tinyxml2::XMLElement* xml);
};

//==============================================================================
// implementation
//==============================================================================
inline
UIElement*  UIBuilderScreen::GetElement(const char* handle)
{
  return m_builder.GetElement(handle);
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

} // xr

#endif // XR_UIBUILDERSCREEN_HPP
