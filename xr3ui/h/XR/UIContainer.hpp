//
// Nuclear Heart Games
// XRhodes
//
// UIContainer.hpp
// @author  Gyorgy Straub
// @date    21/06/2012
//
// copyright (c) 2011 - 2013. All rights reserved.
//
//==============================================================================
#if !defined XR_UICONTAINER_HPP
#define XR_UICONTAINER_HPP

#include <list>
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
  bool          AddElement(UIElement* pElem);
  bool          RemoveElement(UIElement* pElem);
  bool          MoveUpElement(UIElement* pElem);

  int           GetNumElements() const;

  void          RemoveAllElements();

  void          SetWidthToContent();
  void          SetHeightToContent();

  void          SetSizeToContent();

  virtual void  Render() const;
  virtual void  Render(UIRenderer* pRenderer) const;

  virtual void  OnChange();

  virtual bool  OnMouseAction(const Input::MouseActionEvent& e);
  virtual bool  OnMouseMotion(const Input::MouseMotionEvent& e);

protected:
  // types
  typedef std::list<UIElement*> ElementList;

  // data
  ElementList m_lElements; // no ownership

  // internal use
  virtual void  _AlignElement(UIElement* pElem);
  
  virtual void  _SetWidthToContent();
  virtual void  _SetHeightToContent();

  void          _RealignElements(ElementList::iterator i);
  void          _AddChild( UIElement* pElem );
};

//==============================================================================
// implementation
//==============================================================================
inline
int UIContainer::GetNumElements() const
{
  return m_lElements.size();
}

} // XR

#endif // XR_UICONTAINER_HPP
