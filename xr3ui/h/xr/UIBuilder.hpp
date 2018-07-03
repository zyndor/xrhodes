#ifndef XR_UIBUILDER_HPP
#define XR_UIBUILDER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <map>
#include <string>
#include "xr/memory.hpp"
#include "xr/Font.hpp"
#include "xr/Sprite.hpp"
#include "UIContainer.hpp"
#include "tinyxml2.h"

namespace xr
{

//==============================================================================
///@brief Provides UI creation from an xml definition, supporting extension by
/// new UI element types and named elements. Offers support for custom allocators,
/// and string processors for visible text and filenames.
class UIBuilder
{
public:
  // types
  ///@brief Allocates and constructs a UIElement.
  using CreateCallback = UIElement*(*)(AllocateCallback allocate, void* userData);

  ///@brief Initializes a UIElement with the given xml definition, container as
  /// parent and builder. Returns true on success, false on failure.
  using InitCallback = bool(*)(tinyxml2::XMLElement* xml, UIElement* uiElem,
    UIContainer* container, const UIBuilder& builder);

  ///@brief Processes the given string. Returns processed string.
  using ProcessStringCallback = std::string(*)(const char* name, void* userData);

  ///@brief Gets a sprite. TODO: interpret image assets and support materials and
  /// texture packs (texturePack:spriteName), then this could be removed.
  using GetSpriteCallback = Sprite const*(*)(char const* name, void* userData);

  struct  Configuration
  {
    int                     maxDepth;
    GetSpriteCallback       getSprite;
    void*                   getSpriteUserData;
    AllocateCallback        allocate;
    void*                   allocateUserData;
    DeallocateCallback      deallocate;
    void*                   deallocateUserData;
    ProcessStringCallback   processText;
    void*                   processTextUserData;
    ProcessStringCallback   formatFileName;
    void*                   formatFileNameUserData;
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
  static const char* const    kAlignValues[kNumAlignValues];
  static const uint32_t       kAlignValueHashes[kNumAlignValues];

  static const char* const    kElementTypes[kNumUIElementTypes];

  static const Configuration  kDefaultConfig;

  static XmlAlignValue GetXmlAlignment(tinyxml2::XMLElement* xml, const char* attribName);

  // structors
  explicit UIBuilder(const Configuration& cfg = kDefaultConfig);
  ~UIBuilder();

  // general use
  ///@brief The current configuration.
  const Configuration&  GetConfiguration() const;

  ///@brief Sets configuration for future Build()s.
  ///@note Call it outside Build() calls only.
  void  SetConfiguration(const Configuration& cfg);

  ///@brief Sets the max depth beyond (inclusive) which elements are not created.
  ///@note Call it outside Build() calls only.
  void  SetMaxDepth(int maxDepth);

  ///@brief Sets allocate callback and its user data.
  ///@note Call it outside Build() calls only.
  void  SetAllocateCallback(AllocateCallback allocate, void* userData);

  ///@brief Sets deallocate callback and its user data.
  ///@note Call it outside Build() calls only.
  void  SetDeallocateCallback(DeallocateCallback allocate, void* userData);

  ///@brief Sets callback for getting sprites, and its user data.
  ///@note Call it outside Build() calls only.
  void  SetGetSpriteCallback(GetSpriteCallback getSprite, void* userData);

  ///@brief Allocates @a size bytes using the current allocator.
  void* Allocate(int size);

  ///@brief Deallocates @a buffer using the current deallocator.
  void  Deallocate(void* buffer);

  ///@brief Attempts to get sprite using the current GetSprite callback.
  ///@return Const pointer to Sprite if successful, nullptr if not.
  const Sprite* GetSprite(const char* name) const;

  ///@brief Passes @a text to the ProcessText callback.
  ///@return Processed text.
  std::string   ProcessText(const char* text) const;

  ///@brief Registers Create and Initialize callbacks for a UIElement type to be
  /// referred to as @ a name.
  ///@note Will override any previously set creators.
  void  RegisterCreator(const char* name, CreateCallback createCb,
    InitCallback initCb, bool isContainer);

  ///@brief Registers a named element, if one with the value that @a name hashes
  /// to has not yet been registered.
  ///@return The success of the operation, i.e. whether it resulted in registration. 
  bool  RegisterNamedElement(const char* name, UIElement* uiElem);

  ///@brief Attempts to build UI from the given @a xml definition. Children at the
  /// first level will be added to @a container.
  ///@return The success of the operation.
  bool  Build(tinyxml2::XMLElement* xml, UIContainer& container);

  ///@return The element registered with the given @a hash. Nullptr if there isn't
  /// such element.
  UIElement*  GetElement(uint32_t hash) const;

  ///@return The element registered with the value that @a id hashes to. Nullptr
  /// if there isn't such element.
  UIElement*  GetElement(const char* id) const;

  ///@brief Removes, deallocates and destroys all elements in the hierarchy that
  /// it knows about (i.e. added to either of the UIContainer objects which were
  /// created by UIBuilder::Build()).
  void  Destroy();

protected:
  // types
  struct UICreatorRecord
  {
    CreateCallback  createCb;
    InitCallback    initCb;
    bool            isContainer;

#if defined XR_DEBUG
    const char* name;
#endif
  };

  typedef std::list<UIElement*> UIElementList;

  typedef std::map<uint32_t, UICreatorRecord> CreatorMap;
  typedef std::map<uint32_t, UIElement*>      ElementMap;

  // internal
  bool  BuildInternal(tinyxml2::XMLElement* xml, UIContainer* container, int& depth);

  void  PostProcess(tinyxml2::XMLElement* xml, UIElement* uiElem);
  void  PostProcessContainer(tinyxml2::XMLElement* xml, UIContainer* container);

  // data
  CreatorMap          m_creators;
  Configuration       m_cfg;

  UIContainer*        m_root;
  int                 m_depth;
  UIElementList*      m_levels;

  ElementMap          m_namedElements;
};

//==============================================================================
void* NewAllocate(size_t size, void* userData);
void  NewDeallocate(void* buffer, void* userData);

//==============================================================================
UIElement* UIBCreateUISpacer(AllocateCallback allocCb, void* userData);
UIElement* UIBCreateUILabel(AllocateCallback allocCb, void* userData);
UIElement* UIBCreateUIImage(AllocateCallback allocCb, void* userData);
UIElement* UIBCreateUIImagePanel(AllocateCallback allocCb, void* userData);
UIElement* UIBCreateUIHorizontalProgressBar(AllocateCallback allocCb,
  void* userData);
UIElement* UIBCreateUIVerticalProgressBar(AllocateCallback allocCb,
  void* userData);
UIElement* UIBCreateUIHorizontalSlider(AllocateCallback allocCb, void* userData);
UIElement* UIBCreateUIVerticalSlider(AllocateCallback allocCb,
  void* userData);
UIElement* UIBCreateUIButton(AllocateCallback allocCb, void* userData);
UIElement* UIBCreateUICheckBox(AllocateCallback allocCb, void* userData);
UIElement* UIBCreateUIRadioButton(AllocateCallback allocCb, void* userData);
UIElement* UIBCreateUIAligner(AllocateCallback allocCb, void* userData);
UIElement* UIBCreateUICascader(AllocateCallback allocCb, void* userData);
UIElement* UIBCreateUIHorizontalLayout(AllocateCallback allocCb,
  void* userData);
UIElement* UIBCreateUIVerticalLayout(AllocateCallback allocCb, void* userData);
UIElement* UIBCreateUIHorizontalScrollingLayout(AllocateCallback allocCb,
  void* userData);
UIElement* UIBCreateUIVerticalScrollingLayout(AllocateCallback allocCb,
  void* userData);
UIElement* UIBCreateUIGridLayout(AllocateCallback allocCb, void* userData);

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
bool UIBInitUIElement(tinyxml2::XMLElement* xml, UIElement* uiElem,
  UIContainer* parent, const UIBuilder* builder);

//==============================================================================
///@brief Initialises a UIColoredElement from xml. Supported attributes:
/// color: color in RRGGBB format, where each of R, G, and B is a hexadecimal
///  digit.
///@note Passes the processing to UIBInitUIElement, therefore the
/// attributes listed there are handled.
bool UIBInitUIColoredElement(tinyxml2::XMLElement* xml, UIElement* uiElem,
  UIContainer* parent, const UIBuilder* builder);

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
bool UIBInitUILabel(tinyxml2::XMLElement* xml, UIElement* uiElem,
  UIContainer* parent, const UIBuilder* builder);

//==============================================================================
///@brief Initialises a UIImage from xml. Supported attributes:
/// img: the name of the sprite to use. Compulsory.
///@note Passes the processing to UIBInitUIColoredElement, therefore the
/// attributes listed there are handled.
bool UIBInitUIImage(tinyxml2::XMLElement* xml, UIElement* uiElem,
  UIContainer* parent, const UIBuilder* builder);

//==============================================================================
///@brief Initialises a UIImagePanel from xml. Supported attributes:
/// hSplit: horizontal split as percentage of the image width. Defaults to .5.
/// vSplit: vertical split as percentage of the image height. Defaults to .5.
///@note Passes the processing to UIBInitUIImage, therefore the
/// attributes listed there are handled.
bool UIBInitUIImagePanel(tinyxml2::XMLElement* xml, UIElement* uiElem,
  UIContainer* parent, const UIBuilder* builder);

//==============================================================================
///@brief Initialises a UIProgressBarBase from xml. Used for both horizontal
/// and vertical progress bars. Supported attributes:
/// fillDir: the fill direction of the progress bar along screenspace axes.
///  Accepted values: positive/negative.
/// percent: the initial value on the progress bar. Values are clamped in the
///  [.0, 1.0] range.
///@note Passes the processing to UIBInitUIImage, therefore the
/// attributes listed there are handled.
bool UIBInitUIProgressBarBase(tinyxml2::XMLElement* xml, UIElement* uiElem,
  UIContainer* parent, const UIBuilder* builder);

///@brief Initialises a UISliderBase from xml. Supported attributes:
/// imgSlider: the name of the sprite to use as the slider. Compulsory.
///@note One of the bgMaterial and uv attributes is compulsory.
///@note Passes the processing to UIBInitUIImage, therefore the
/// attributes listed there are handled.
bool UIBInitUISliderBase(tinyxml2::XMLElement* xml, UIElement* uiElem,
  UIContainer* parent, const UIBuilder* builder);

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
bool UIBInitUIButton(tinyxml2::XMLElement* xml, UIElement* uiElem,
  UIContainer* parent, const UIBuilder* builder);

///@brief Initialises a UICheckBox from xml. Supported attributes:
/// selected: set to >0 / y{...} / Y{...} to check the box.
/// imgSet: the sprite to superimpose it when the box is checked.
///@note Passes the processing to UIBInitUIButton, therefore the
/// attributes listed there are handled.
bool UIBInitUICheckBox(tinyxml2::XMLElement* xml, UIElement* uiElem,
  UIContainer* parent, const UIBuilder* builder);

///@brief Initialises a UIRadioButton from xml. Supported attributes:
/// group: the name of the UIRadioButton::Group to add the button to.
///@note Passes the processing to UIBInitUIColoredElement, therefore the
/// attributes listed there are handled.
bool UIBInitUIRadioButton(tinyxml2::XMLElement* xml, UIElement* uiElem,
  UIContainer* parent, const UIBuilder* builder);

///@brief Initialises a UIAligner from xml. Supported attributes:
/// hAlign: horizontal alignment applied to the child elements. Accepted values
///  are: low/center/high/none.
/// vAlign: vertical alignment applied to the child elements. Accepted values
///  are: low/center/high/none.
///@note Passes the processing to UIBInitUIElement, therefore the
/// attributes listed there are handled.
bool UIBInitUIAligner(tinyxml2::XMLElement* xml, UIElement* uiElem,
  UIContainer* parent, const UIBuilder* builder);

///@brief Initialises a UICascader from xml. Supported attributes:
/// hOffset: horizontal offset of cascading. *Base: own width.
/// vOffset: vertical offset of cascading. *Base: own height.
/// *The notations #%, #.# and #x are supported and are used for scaling the
/// value in the xml by Base (in the case of #%, percent scaling, meaning that
/// the 1/100th of the value is used).
///@note Passes the processing to UIBInitUIAligner, therefore the
/// attributes listed there are handled.
bool UIBInitUICascader(tinyxml2::XMLElement* xml, UIElement* uiElem,
  UIContainer* parent, const UIBuilder* builder);

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
bool UIBInitUIGrowingLayout(tinyxml2::XMLElement* xml, UIElement* uiElem,
  UIContainer* parent, const UIBuilder* builder);

///@brief Initialises a UIVerticalScrollingLayout from xml. Supported
/// attributes:
/// yOffset: the vertical offset into the container.
///@note Passes the processing to UIBInitUIGrowingLayout, therefore the
/// attributes listed there are handled.
bool UIBInitUIVerticalScrollingLayout(tinyxml2::XMLElement* xml, UIElement* uiElem,
  UIContainer* parent, const UIBuilder* builder);

///@brief Initialises a UIHorizontalScrollingLayout from xml. Supported
/// attributes:
/// xOffset: the horoizontal offset into the container.
///@note Passes the processing to UIBInitUIGrowingLayout, therefore the
/// attributes listed there are handled.
bool UIBInitUIHorizontalScrollingLayout(tinyxml2::XMLElement* xml, UIElement* uiElem,
  UIContainer* parent, const UIBuilder* builder);

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
bool UIBInitUIGridLayout(tinyxml2::XMLElement* xml, UIElement* uiElem,
  UIContainer* parent, const UIBuilder* builder);

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

} // xr

#endif // XR_UIBUILDER_HPP
