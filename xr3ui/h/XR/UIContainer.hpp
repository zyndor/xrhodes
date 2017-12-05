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

namespace XR
{

//==============================================================================
class UIContainer:  public UIElement
{
public:
  // structors
  UIContainer();
  ~UIContainer();

  // general
  bool                    AddElement(UIElement* pElem);
  bool                    RemoveElement(UIElement* pElem);
  bool                    MoveUpElement(UIElement* pElem);

  const UIElement::List&  GetElements() const;
  size_t                  GetNumElements() const;

  void                    RemoveAllElements();

  void                    SetWidthToContent();
  void                    SetHeightToContent();

  void                    SetSizeToContent();

  virtual void            Render() const;
  virtual void            Render(UIRenderer* pRenderer) const;

  virtual void            OnChange();

  virtual bool            OnMouseAction(const Input::MouseActionEvent& e);
  virtual bool            OnMouseMotion(const Input::MouseMotionEvent& e);

protected:
  // data
  UIElement::List m_lElements; // no ownership

  // internal use
  virtual void  _AlignElement(UIElement* pElem);
  
  virtual void  _SetWidthToContent();
  virtual void  _SetHeightToContent();

  void          _RealignElements(UIElement::List::iterator i);
  void          _AddChild( UIElement* pElem );
};

//==============================================================================
// implementation
//==============================================================================
inline
size_t UIContainer::GetNumElements() const
{
  return m_lElements.size();
}

//==============================================================================
inline
const UIElement::List&  UIContainer::GetElements() const
{
  return m_lElements;
}

} // XR

#endif // XR_UICONTAINER_HPP
