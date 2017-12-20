#ifndef XR_UIELEMENT_HPP
#define XR_UIELEMENT_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <list>
#include "XR/utils.hpp"
#include "XR/Rect.hpp"
#include "XR/Input.hpp"

namespace XR
{

//==============================================================================
class UIContainer;
class UIEventNotifier;
class IUIRenderer;

//==============================================================================
class UIElement:  public Rect
{
public:
  // types
  enum  Alignment
  {
    AL_LOW = -1, // as in on the axis; left / top
    AL_CENTER,
    AL_HIGH,  // as in on the axis; right / bottom
    AL_NOALIGN
  };

  typedef std::list<UIElement*> List;

  // static
  static void  OnChangeCaller(void* pData);

  // structors
  UIElement();
  UIElement(int w, int h);
  virtual ~UIElement();

  // virtual
  ///@brief Renders the UIElement using a UIRenderer.
  virtual void Render(IUIRenderer& renderer) const =0;

  // general
  ///@return  A pointer to the container the UIElement is added to, if any.
  UIContainer*  GetParent() const;

  ///@brief Sets the UIContainer pointed to by @a pContainer as the parent of
  /// this UIElement.
  void          SetParent(UIContainer* pContainer);

  ///@brief Calculates the x coordinate of the UIElement's right side based on
  /// position and width.
  int           CalculateRight() const;

  ///@brief Calculates the y coordinate of the UIElement's bottom side based on
  /// position and height.
  int           CalculateBottom() const;

  ///@brief Sets the position of the UIElement and calls OnChange().
  void          SetPosition(int x1, int y1);

  ///@brief Sets the size of the UIElement and calls OnChange().
  void          SetSize(int w1, int h1);

  ///@brief Convenience method to set the size of the element to another
  /// element.
  void          Resize(const UIElement* pElem);

  ///@brief Aligns the element horizontally to the given x coordinate and with
  /// the given alignment, then calls OnChange().
  void          AlignHorizontally(int x1, Alignment hAlign);

  ///@brief Aligns the element vertically to the given y coordinate and with
  /// the given alignment, then calls OnChange().
  void          AlignVertically(int y1, Alignment vAlign);

  ///@brief Aligns the element along both axes to the given coordinates and
  /// with the given alignment, then calls OnChange().
  void          Align(int x1, int y1, Alignment hAlign, Alignment vAlign);

  ///@brief Convenience method to center the UIElement horizontally on
  /// screen.
  void          CenterHorizontally();

  ///@brief Convenience method to center the UIElement vertically on the
  /// screen.
  void          CenterVertically();

  ///@brief Convenience method to center the UIElement on the screen.
  void          Center();

  ///@brief Offsets the position of the UIElement by @a vx and @a vy and calls
  /// OnChage().
  void          Move(int vx, int vy);

  ///@brief Convenience method to register the element on a UIEventDispatcher
  /// for MOUSE_ACTION_EVENT and MOUSE_MOTION_EVENT.
  bool          Register(UIEventNotifier& disp);

  ///@brief Convenience method to unregister the element on a
  /// UIEventDispatcher for MOUSE_ACTION_EVENT and MOUSE_MOTION_EVENT.
  bool          Unregister(UIEventNotifier& disp);

  ///@brief Processes changes of position and/or size carried out via
  /// direct access to members.
  virtual void  OnChange();

  ///@brief Handles an MOUSE_ACTION_EVENT.
  ///@return  Whether the dispatcher should stop propagating the event.
  virtual bool  OnMouseAction(const Input::MouseActionEvent& e);

  ///@brief Handles an MOUSE_MOTION_EVENT.
  ///@return  Whether the dispatcher should stop propagating the event.
  virtual bool  OnMouseMotion(const Input::MouseMotionEvent& e);

protected:
  // data
  UIContainer*  m_pParent;

  // internal
  ///@brief Centers the UIElement horizontally on the screen without calling
  /// OnChange().
  void  _CenterHorizontally();

  ///@brief Centers the UIElement vertically on the screen without calling
  /// OnChange().
  void  _CenterVertically();

  ///@brief Aligns the element horizontally to the given x coordinate and with
  /// the given alignment, without calling OnChange().
  void  _AlignHorizontally(int x1, Alignment hAlign);

  ///@brief Aligns the element vertically to the given x coordinate and with
  /// the given alignment, without calling OnChange().
  void  _AlignVertically(int y1, Alignment vAlign);
};

//==============================================================================
// implementation
//==============================================================================
inline
int UIElement::CalculateRight() const
{
  return x + w;
}

//==============================================================================
inline
int UIElement::CalculateBottom() const
{
  return y + h;
}

//==============================================================================
inline
UIContainer* UIElement::GetParent() const
{
  return m_pParent;
}

} // XR

#endif // XR_UIELEMENT_HPP
