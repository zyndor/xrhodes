//
// Nuclear Heart Games
// XRhodes
//
// UIState.hpp
// @author  Gyorgy Straub
// @date    21/06/2012
//
// copyright (c) 2011 - 2015. All rights reserved.
//
//==============================================================================
#if !defined XR_UISTATE_HPP
#define XR_UISTATE_HPP

#include <XR/State.hpp>

#include "UIEventDispatcher.hpp"

#include "UIRadioButton.hpp"
#include "UIImagePanel.hpp"
#include "UILabel.hpp"
#include "UISpacer.hpp"
#include "UICascader.hpp"
#include "UIHorizontalScrollingLayout.hpp"
#include "UIHorizontalProgressBar.hpp"
#include "UIHorizontalSlider.hpp"
#include "UIVerticalScrollingLayout.hpp"
#include "UIVerticalProgressBar.hpp"
#include "UIVerticalSlider.hpp"
#include "UIGridLayout.hpp"

#include "UIRenderer.hpp"

namespace XR
{

//==============================================================================
///@brief UIState manages a list of modalities which are root UI containers
/// and UIEventDispatchers that you can add to and register your UI to. An
/// initial modality is automatically created on Init() and all modalities are
/// destroyed on Exit().
class UIState:  public State
{
public:
  // structors
  UIState();
  virtual ~UIState();
  
  // general
  UIRenderer*					GetRenderer() const;  // no ownership transfer
  void								SetRenderer(UIRenderer* pRenderer);  // no ownership transfer

	virtual void				Init();
	virtual void				Update(int32 ms, Result& result);
	virtual void				Render();
	virtual void				Shutdown();
	
protected:
  // typedef
  typedef void(*Callback)(void*);
  
  // static
  static UIState*			s_pActiveInstance;

  // virtual
  virtual void				_Init() =0;
  virtual void				_Shutdown() =0;
  virtual void				_Update(int32 ms, Result& result) =0;
  virtual void				_Render();

	// internal
  UIContainer&        GetUIRoot();
  UIEventDispatcher&	GetDispatcher();
  
  void								PushModality(Callback pOnModalityPopCb, void* pOnModalityPopCbData);
  void								RequestModalityPop();

  virtual void				_OnModalityRenderStart( int modalitiesRendered );

private:
  // typedef
  struct  Modality
  {
    UIContainer        container;
    UIEventDispatcher	dispatcher;
    Callback          pOnModalityPopCb;
    void*             pOnModalityPopCbData;
  };
  
  typedef std::list<Modality> ModalityList;

  // internal
  void								PopModality();

  // data
  ModalityList				m_modalities;
  bool								m_isModalityPopRequested;
  
  UIRenderer*					m_pRenderer;  // no ownership
};	

//==============================================================================
// implementation
//==============================================================================
inline
UIContainer& UIState::GetUIRoot()
{
  XR_ASSERTMSG(XR::UIstate, m_modalities.size() > 0,
    ("There aren't any modalities available in UIState."));
  return m_modalities.back().container;
}

//==============================================================================
inline
UIEventDispatcher& UIState::GetDispatcher()
{
  XR_ASSERTMSG(XR::UIstate, m_modalities.size() > 0,
    ("There aren't any dispatchers available in UIState."));
  return m_modalities.back().dispatcher;
}

//==============================================================================
inline
UIRenderer* UIState::GetRenderer() const
{
  return m_pRenderer;
}

} // XR

#endif // XR_UISTATE_HPP
