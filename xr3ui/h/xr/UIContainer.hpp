#ifndef XR_UICONTAINER_HPP
#define XR_UICONTAINER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "UIElement.hpp"

namespace xr
{

//==============================================================================
///@brief Base class for UIElements that may have other elements as children.
/// External logic is required to take care of the ownership of these child
/// elements. The Container does not perform spatial organization of its
/// children - its specialisations do.
class UIContainer:  public UIElement
{
public:
  // structors
  UIContainer();

  // general
  ///@brief Adds an element as a child to the container, if not already a child.
  ///@return Whether the element was successfully added.
  ///@note No ownership transfer.
  ///@note @a elem is removed from any previous parent it may have.
  bool  AddElement(UIElement* elem);

  ///@brief Removes an element from the container, if it is a child.
  ///@return Whether the element was succesfully removed.
  ///@note No ownership transfer.
  bool  RemoveElement(UIElement* elem);

  ///@brief Removes, then adds the element.
  ///@note No ownership transfer.
  bool  MoveUpElement(UIElement* elem);

  ///@return The list of elements.
  ///@note No ownership transfer.
  const UIElement::List&  GetElements() const;

  ///@return The number of elements.
  size_t  GetNumElements() const;

  ///@brief Removes all child elements from the container.
  void  RemoveAllElements();

  ///@brief Updates the width of the container base on its content.
  void  SetWidthToContent();

  ///@brief Updates the height of the container base on its content.
  void  SetHeightToContent();

  ///@brief Updates both width and height of the container base on its content.
  void  SetSizeToContent();

  void  Render(IUIRenderer& renderer) const override;

  void  OnChange() override;

  bool  OnMouseAction(const Input::MouseActionEvent& e) override;
  bool  OnMouseMotion(const Input::MouseMotionEvent& e) override;

protected:
  // data
  UIElement::List m_children; // no ownership

  // internal use
  ///@brief Performs the positioning of a child element, typically at the point
  /// of adding to the container, within the containers.
  virtual void  AlignChildImpl(UIElement* elem);

  ///@brief Performs the calculation for the width of the container.
  virtual void  SetWidthToContentImpl();

  ///@brief Performs the calculation for the height of the container.
  virtual void  SetHeightToContentImpl();

  ///@brief Recalculates the position of all elements starting at @a i into m_elements.
  void  RealignChildren(UIElement::List::iterator i);

  ///@brief Appends a child element to m_elements and calls AlignElementImpl() on it.
  void  AddChild( UIElement* elem );
};

//==============================================================================
// implementation
//==============================================================================
inline
size_t UIContainer::GetNumElements() const
{
  return m_children.size();
}

//==============================================================================
inline
const UIElement::List&  UIContainer::GetElements() const
{
  return m_children;
}

} // xr

#endif // XR_UICONTAINER_HPP
