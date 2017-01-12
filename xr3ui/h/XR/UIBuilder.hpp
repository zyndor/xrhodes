//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_UIBUILDER_HPP
#define XR_UIBUILDER_HPP

#include <map>
#include <string>
#include <XR/memory.hpp>
#include <tinyxml.h>
#include <XR/Font.hpp>
#include "UIContainer.hpp"

namespace XR
{

//==============================================================================
class UIBuilder
{
public:
  // types
  typedef UIElement*(*CreateCallback)(AllocateCallback pAllocate, void* pUser);
  typedef bool(*InitCallback)(TiXmlElement* pXml, UIElement* pUIElem,
    UIContainer* pContainer, const UIBuilder& builder);
  typedef std::string(*ProcessStringCallback)(const char* pName, void* pUser);

  struct  Configuration
  {
    int                     maxDepth;
    Sprite::GetCallback     pGetSprite;
    void*                   pGetSpriteData;
    Font::GetCallback       pGetFont;
    void*                   pGetFontData;
    AllocateCallback        pAllocate;
    void*                   pAllocateData;
    DeallocateCallback      pDeallocate;
    void*                   pDeallocateData;
    ProcessStringCallback   pProcessText;
    void*                   pProcessTextUser;
    ProcessStringCallback   pFormatFileName;
    void*                   pFormatFileNameUser;
  };
  
  enum  XmlAlignValue
  {
    XA_LOW = UIElement::AL_LOW + 1,
    XA_CENTER,
    XA_HIGH,
    XA_NONE,
    kNumAlignValues
  };

  enum UIElementType
  {
    UI_SPACER,
    UI_BUTTON,
    UI_CHECKBOX,
    UI_RADIOBUTTON,
    UI_LABEL,
    UI_IMAGE,
    UI_IMAGEPANEL,
    UI_HPROGRESS,
    UI_VPROGRESS,
    UI_HSLIDER,
    UI_VSLIDER,
    UI_ALIGNER,
    UI_CASCADER,
    UI_HLAYOUT,
    UI_VLAYOUT,
    UI_HSCROLLINGLAYOUT,
    UI_VSCROLLINGLAYOUT,
    UI_GRIDLAYOUT,
    kNumUIElementTypes
  };

  // static
  static const char* const    kInclude;
  static const uint32_t       kIncludeHash;
  static const char* const    karpAlignValues[kNumAlignValues];
  static const uint32_t       karAlignValueHash[kNumAlignValues];

  static const char* const    karpElementName[kNumUIElementTypes];

  static const Configuration  kDefaultConfig;

  static int    GetXmlAlignment(TiXmlElement* pXml, const char* pAttribName);
 
  // structors
  explicit UIBuilder(const Configuration& cfg = kDefaultConfig);
  ~UIBuilder();

  // general use
  const Configuration&  GetConfiguration() const;
  void                  SetConfiguration(const Configuration& cfg);
  
  void          SetMaxDepth(int maxDepth);

  void          SetAllocateCallback(AllocateCallback pAllocateCb, void* pCbData);
  void          SetDeallocateCallback(DeallocateCallback pAllocateCb, void* pCbData);
  void          SetGetSpriteCallback(Sprite::GetCallback pGetSpriteCb, void* pCbData);
  void          SetGetFontCallback(Font::GetCallback pGetFontCb, void* pCbData);

  void*         Allocate(int size);
  void          Deallocate(void* pBuffer);
  const Sprite* GetSprite(const char* pName) const;
  const Font*   GetFont(const char* pName) const;
  std::string   ProcessText(const char* pText) const;
  
  void          RegisterCreator(const char* pName, CreateCallback pCreateCb,
                  InitCallback pInitCb, bool isContainer);

  bool          RegisterNamedElement(const char* pName, UIElement* pUIElem);

  bool          Build(TiXmlElement* pXml, UIContainer& container);

  UIElement*    GetElement(uint32_t hash) const;
  UIElement*    GetElement(const char* pHandle) const;

  void          Destroy();

protected:
  // types
  struct UICreatorRecord
  {
    CreateCallback  pCreateCb;
    InitCallback    pInitCb;
    bool            isContainer;

#if defined XR_DEBUG
    const char* pName;
#endif
  };

  typedef std::list<UIElement*> UIElementList;

  typedef std::map<uint32_t, UICreatorRecord> CreatorMap;
  typedef std::map<uint32_t, UIElement*>      ElementMap;

  // internal
  bool  _Build(TiXmlElement* pXml, UIContainer* pContainer, int& depth);

  void  _PostProcess(TiXmlElement* pXml, UIElement* pUIElem);
  void  _PostProcessContainer(TiXmlElement* pXml, UIContainer* pUIContainer);

  // data
  CreatorMap          m_creators;
  Configuration       m_cfg;

  UIContainer*        m_pRoot;
  int                 m_depth;
  UIElementList*      m_parLevels;

  ElementMap          m_handles;
};

//==============================================================================
void* NewAllocate(size_t size, void* pUser);
void  NewDeallocate(void* pMem, void* pUser);

//==============================================================================
UIElement* UIBCreateUISpacer(AllocateCallback pAllocCb, void* pAllocCbData);
UIElement* UIBCreateUILabel(AllocateCallback pAllocCb, void* pAllocCbData);
UIElement* UIBCreateUIImage(AllocateCallback pAllocCb, void* pAllocCbData);
UIElement* UIBCreateUIImagePanel(AllocateCallback pAllocCb, void* pAllocCbData);
UIElement* UIBCreateUIHorizontalProgressBar(AllocateCallback pAllocCb,
  void* pAllocCbData);
UIElement* UIBCreateUIVerticalProgressBar(AllocateCallback pAllocCb,
  void* pAllocCbData);
UIElement* UIBCreateUIHorizontalSlider(AllocateCallback pAllocCb,
  void* pAllocCbData);
UIElement* UIBCreateUIVerticalSlider(AllocateCallback pAllocCb,
  void* pAllocCbData);
UIElement* UIBCreateUIButton(AllocateCallback pAllocCb, void* pAllocCbData);
UIElement* UIBCreateUICheckBox(AllocateCallback pAllocCb, void* pAllocCbData);
UIElement* UIBCreateUIRadioButton(AllocateCallback pAllocCb, void* pAllocCbData);
UIElement* UIBCreateUIAligner(AllocateCallback pAllocCb, void* pAllocCbData);
UIElement* UIBCreateUICascader(AllocateCallback pAllocCb, void* pAllocCbData);
UIElement* UIBCreateUIHorizontalLayout(AllocateCallback pAllocCb,
  void* pAllocCbData);
UIElement* UIBCreateUIVerticalLayout(AllocateCallback pAllocCb,
  void* pAllocCbData);
UIElement* UIBCreateUIHorizontalScrollingLayout(AllocateCallback pAllocCb,
  void* pAllocCbData);
UIElement* UIBCreateUIVerticalScrollingLayout(AllocateCallback pAllocCb,
  void* pAllocCbData);
UIElement* UIBCreateUIGridLayout(AllocateCallback pAllocCb, void* pAllocCbData);

//==============================================================================
/// PLEASE NOTE THAT THE ATTRIBUTE NAMES ARE CASE SENSITIVE.
//==============================================================================
///@brief Initialises a UIElement from xml. Supported attributes:
/// x: the x position of the element in screenspace. Base*: screen width.
/// y: the y position of the element in screenspace. Base*: screen height.
/// w: the width of the element in screenspace. Base*: parent's width.
/// h: the height of the element in screenspace. Base*: parent's height.
/// *The notations #%, #.# and #x are supported and are used for scaling the
/// value in the xml by Base (in the case of #%, percent scaling, meaning that
/// the 1/100th of the value is used).
bool UIBInitUIElement(TiXmlElement* pXml, UIElement* pUIElem,
  UIContainer* pParent, const UIBuilder* pBuilder);

//==============================================================================
///@brief Initialises a UIColoredElement from xml. Supported attributes:
/// color: color in RRGGBB format, where each of R, G, and B is a hexadecimal
///  digit.
///@note Passes the processing to UIBInitUIElement, therefore the
/// attributes listed there are handled.
bool UIBInitUIColoredElement(TiXmlElement* pXml, UIElement* pUIElem,
  UIContainer* pParent, const UIBuilder* pBuilder);

//==============================================================================
///@brief Initialises a UILabel from xml. The content of the element is used to
/// prepare the text of the label. Supported attributes:
/// font: the name of the font.
/// hAlign: horizontal alignment of text within width. Accepted values are:
///  left/center/right.
/// vAlign: vertical alignment of text within height. Accepted values are:
///  top/middle/bottom.
/// sizeToContent: defines whether the label's size should be set to its
///  contents. Accepts any combination of the w (sets the width) and h (sets
///  the height) characters.
///@note Passes the processing to UIBInitUIColoredElement, therefore the
/// attributes listed there are handled.
bool UIBInitUILabel(TiXmlElement* pXml, UIElement* pUIElem,
  UIContainer* pParent, const UIBuilder* pBuilder);

//==============================================================================
///@brief Initialises a UIImage from xml. Supported attributes:
/// img: the name of the sprite to use. Compulsory.
///@note Passes the processing to UIBInitUIColoredElement, therefore the
/// attributes listed there are handled.
bool UIBInitUIImage(TiXmlElement* pXml, UIElement* pUIElem,
  UIContainer* pParent, const UIBuilder* pBuilder);

//==============================================================================
///@brief Initialises a UIImagePanel from xml. Supported attributes:
/// hSplit: horizontal split as percentage of the image width. Defaults to .5.
/// vSplit: vertical split as percentage of the image height. Defaults to .5.
///@note Passes the processing to UIBInitUIImage, therefore the
/// attributes listed there are handled.
bool UIBInitUIImagePanel(TiXmlElement* pXml, UIElement* pUIElem,
  UIContainer* pParent, const UIBuilder* pBuilder);

//==============================================================================
///@brief Initialises a UIProgressBarBase from xml. Used for both horizontal
/// and vertical progress bars. Supported attributes:
/// fillDir: the fill direction of the progress bar along screenspace axes.
///  Accepted values: positive/negative.
/// percent: the initial value on the progress bar. Values are clamped in the
///  [.0, 1.0] range.
///@note Passes the processing to UIBInitUIImage, therefore the
/// attributes listed there are handled.
bool UIBInitUIProgressBarBase(TiXmlElement* pXml, UIElement* pUIElem,
  UIContainer* pParent, const UIBuilder* pBuilder);

///@brief Initialises a UISliderBase from xml. Supported attributes:
/// imgSlider: the name of the sprite to use as the slider. Compulsory.
///@note One of the bgMaterial and uv attributes is compulsory.
///@note Passes the processing to UIBInitUIImage, therefore the
/// attributes listed there are handled.
bool UIBInitUISliderBase(TiXmlElement* pXml, UIElement* pUIElem,
  UIContainer* pParent, const UIBuilder* pBuilder);

///@brief Initialises a UIButton from xml. Supported attributes:
/// enabled: set to 0 / n{...} / N{...} to disable the button. The default is
///  true.
/// img or imgUp: the name of the sprite to use as the unpressed state of the
///  button. Compulsory.
/// imgDown: the name of the sprite to use as the pressed state of the button.
///  If not defined, will default to the value of img{Up}.
/// imgOff: the name of the sprite to use as the disabled state of the button.
///  If not defined, will default to the value of imgDown.
///@note Passes the processing to UIBInitUIColoredElement, therefore the
/// attributes listed there are handled.
bool UIBInitUIButton(TiXmlElement* pXml, UIElement* pUIElem,
  UIContainer* pParent, const UIBuilder* pBuilder);

///@brief Initialises a UICheckBox from xml. Supported attributes:
/// selected: set to >0 / y{...} / Y{...} to check the box.
/// imgSet: the sprite to superimpose it when the box is checked.
///@note Passes the processing to UIBInitUIButton, therefore the
/// attributes listed there are handled.
bool UIBInitUICheckBox(TiXmlElement* pXml, UIElement* pUIElem,
  UIContainer* pParent, const UIBuilder* pBuilder);
    
///@brief Initialises a UIRadioButton from xml. Supported attributes:
/// group: the name of the UIRadioButton::Group to add the button to.
///@note Passes the processing to UIBInitUIColoredElement, therefore the
/// attributes listed there are handled.
bool UIBInitUIRadioButton(TiXmlElement* pXml, UIElement* pUIElem,
  UIContainer* pParent, const UIBuilder* pBuilder);

///@brief Initialises a UIAligner from xml. Supported attributes:
/// hAlign: horizontal alignment applied to the child elements. Accepted values
///  are: low/center/high/none.
/// vAlign: vertical alignment applied to the child elements. Accepted values
///  are: low/center/high/none.
///@note Passes the processing to UIBInitUIElement, therefore the
/// attributes listed there are handled.
bool UIBInitUIAligner(TiXmlElement* pXml, UIElement* pUIElem,
  UIContainer* pParent, const UIBuilder* pBuilder);

///@brief Initialises a UICascader from xml. Supported attributes:
/// hOffset: horizontal offset of cascading. *Base: own width.
/// vOffset: vertical offset of cascading. *Base: own height.
/// *The notations #%, #.# and #x are supported and are used for scaling the
/// value in the xml by Base (in the case of #%, percent scaling, meaning that
/// the 1/100th of the value is used).
///@note Passes the processing to UIBInitUIAligner, therefore the
/// attributes listed there are handled.
bool UIBInitUICascader(TiXmlElement* pXml, UIElement* pUIElem,
  UIContainer* pParent, const UIBuilder* pBuilder);

///@brief Initialises a UIGrowingLayout from xml. Supported attributes:
/// align: alignment applied to the child elements. Accepted values are:
///  low/center/high/none. (For UIHorizontalLayout, this is vertical alignment;
///  for UIVerticalLayout this is horizontal alignment)
/// growDir: the direction the container is growing in terms of the screenspace
///  axed. Accepted values are: positive/negative.
/// spacing: the amount of pixels between elements. *Base: the default spacing
///  in UIGrowingLayout.
/// *The notations #%, #.# and #x are supported and are used for scaling the
/// value in the xml by Base (in the case of #%, percent scaling, meaning that
/// the 1/100th of the value is used).
///@note Passes the processing to UIBInitUIElement, therefore the
/// attributes listed there are handled.
bool UIBInitUIGrowingLayout(TiXmlElement* pXml, UIElement* pUIElem,
  UIContainer* pParent, const UIBuilder* pBuilder);

///@brief Initialises a UIVerticalScrollingLayout from xml. Supported
/// attributes:
/// yOffset: the vertical offset into the container.
///@note Passes the processing to UIBInitUIGrowingLayout, therefore the
/// attributes listed there are handled.
bool UIBInitUIVerticalScrollingLayout(TiXmlElement* pXml, UIElement* pUIElem,
  UIContainer* pParent, const UIBuilder* pBuilder);

///@brief Initialises a UIHorizontalScrollingLayout from xml. Supported
/// attributes:
/// xOffset: the horoizontal offset into the container.
///@note Passes the processing to UIBInitUIGrowingLayout, therefore the
/// attributes listed there are handled.
bool UIBInitUIHorizontalScrollingLayout(TiXmlElement* pXml, UIElement* pUIElem,
  UIContainer* pParent, const UIBuilder* pBuilder);

///@brief Initialises a UIGridLayout from xml. Supported Attributes:
/// rows: number of rows for the layout.
/// cols: number of columns for the layout.
/// rowSpacing: amount of spacing between rows. *Base: the default row spacing
///  in UIGridLayout.
/// colSpacing: amount of spacing between columns. *Base: the default
///  column spacing in UIGridLayout.
/// hAlign: horizontal alignment of the child elements. Accepted values are:
///  low/center/high/none.
/// hAlign: vertical alignment of the child elements. Accepted values are:
///  low/center/high/none.
/// *The notations #%, #.# and #x are supported and are used for scaling the
/// value in the xml by Base (in the case of #%, percent scaling, meaning that
/// the 1/100th of the value is used).
///@note Passes the processing to UIBInitUIElement, therefore the
/// attributes listed there are handled.
bool UIBInitUIGridLayout(TiXmlElement* pXml, UIElement* pUIElem,
  UIContainer* pParent, const UIBuilder* pBuilder);

/// UIBuilder supports a few miscellaneous attributes. These are the following:
/// handle: an id that you can then use to reference the element. Must be
///  unique.
/// alignTo: its value is an element handle that must already be defined prior
///  to the element you're trying to align.
/// alignToHorizontal: which side of the target will the elements be aligned to
///  horizontally. Accepted values are: low/center/high. Default is low.
///  Ignored if alignTo was not defined.
/// alignToVertical: which side of the target will the elements be aligned to
///  vertically. Accepted values are: low/center/high. Default is low. Ignored
///  if alignTo was not defined.
/// alignSideHorizontal: which horizontal side of the element will be aligned
///  to the target. Accepted values are: low/center/high. Default is low.
///  Ignored if alignTo was not defined.
/// alignSideVertical: which vertical side of the element will be aligned
///  to the target. Accepted values are: low/center/high. Default is low.
///  Ignored if alignTo was not defined.
/// sizeToContent: only processed on container elements. Accepts a combination
///  of the w (affects width) and h (affects height) characters, and sets the
///  given dimension of the container to the maximum of its children's
///  respective dimension(s) after all of the children have been added.

//==============================================================================
// implementation
//==============================================================================
inline
const UIBuilder::Configuration& UIBuilder::GetConfiguration() const
{
  return  m_cfg;
}

} // XR

#endif // XR_UIBUILDER_HPP