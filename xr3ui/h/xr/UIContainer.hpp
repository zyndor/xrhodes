#ifndef XR_UICONTAINER_HPP
#define XR_UICONTAINER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "UIElement.hpp"

namespace xr
{

//==============================================================================
class UIContainer:  public UIElement
{
public:
  // structors
  UIContainer();
  ~UIContainer();

  // general
  bool                    AddElement(UIElement* elem);
  bool                    RemoveElement(UIElement* elem);
  bool                    MoveUpElement(UIElement* elem);

  const UIElement::List&  GetElements() const;
  size_t                  GetNumElements() const;

  void                    RemoveAllElements();

  void                    SetWidthToContent();
  void                    SetHeightToContent();

  void                    SetSizeToContent();

  void  Render(IUIRenderer& renderer) const override;

  void  OnChange() override;

  bool  OnMouseAction(const Input::MouseActionEvent& e) override;
  bool  OnMouseMotion(const Input::MouseMotionEvent& e) override;

protected:
  // data
  UIElement::List m_elements; // no ownership

  // internal use
  virtual void  _AlignElement(UIElement* elem);

  virtual void  _SetWidthToContent();
  virtual void  _SetHeightToContent();

  void          _RealignElements(UIElement::List::iterator i);
  void          _AddChild( UIElement* elem );
};

//==============================================================================
// implementation
//==============================================================================
inline
size_t UIContainer::GetNumElements() const
{
  return m_elements.size();
}

//==============================================================================
inline
const UIElement::List&  UIContainer::GetElements() const
{
  return m_elements;
}

} // xr

#endif // XR_UICONTAINER_HPP
