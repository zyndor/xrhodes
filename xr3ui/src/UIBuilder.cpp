//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/UIBuilder.hpp"
#include "xr/UISpacer.hpp"
#include "xr/UIRadioButton.hpp"
#include "xr/UILabel.hpp"
#include "xr/UIImagePanel.hpp"
#include "xr/UIHorizontalProgressBar.hpp"
#include "xr/UIVerticalProgressBar.hpp"
#include "xr/UIHorizontalSlider.hpp"
#include "xr/UIVerticalSlider.hpp"
#include "xr/UIAligner.hpp"
#include "xr/UICascader.hpp"
#include "xr/UIHorizontalScrollingLayout.hpp"
#include "xr/UIVerticalScrollingLayout.hpp"
#include "xr/UIGridLayout.hpp"
#include "xr/debug.hpp"
#include "xr/Hash.hpp"
#include "xr/Parse.hpp"
#include "xr/Pool.hpp"
#include "xr/utils.hpp"

#define ARRAY_SIZE(x) std::extent<decltype(x)>::value

namespace xr
{

//==============================================================================
static const uint32_t kDirectionNameHashes[] =
{
  Hash::String32("positive"),
  Hash::String32("negative")
};

static const uint32_t kHorizontalAlignNameHashes[] =
{
  Hash::String32("left"),
  Hash::String32("center"),
  Hash::String32("right"),
  //Hash::String("paragraph")
};

static const uint32_t kVerticalAlignNameHashes[] =
{
  Hash::String32("top"),
  Hash::String32("middle"),
  Hash::String32("bottom")
};


//==============================================================================
float GetXmlScaleAttribute(tinyxml2::XMLElement* xml)
{
  XR_ASSERT(UIBuilder, xml != nullptr);
  double  scale = .0;
  xml->QueryDoubleAttribute("scale", &scale);
  if (scale == .0)
  {
    scale = 1.0;
  }
  return static_cast<float>(scale);
}

//==============================================================================
bool  GetXmlIntAttribute(tinyxml2::XMLElement* xml, const char* attribName, int base,
        int& v)
{
  XR_ASSERT(UIBuilder, xml != nullptr);
  XR_ASSERT(UIBuilder, attribName != nullptr);

  const char* value(xml->Attribute(attribName));
  return Parse::Int(base, value, v);
}

//==============================================================================
bool  GetXmlFloatAttribute(tinyxml2::XMLElement* xml, const char* attribName, float& v)
{
  XR_ASSERT(UIBuilder, xml != nullptr);
  XR_ASSERT(UIBuilder, attribName != nullptr);

  const char* value(xml->Attribute(attribName));
  return Parse::Float(value, v);
}

//==============================================================================
// size to content values
enum  XmlDimension
{
  XD_WIDTH,
  XD_HEIGHT,
  kNumXmlDimensions
};

static const int kXmlDimensionChars[kNumXmlDimensions] =
{
  'w',
  'h'
};

uint32_t  GetXmlDimensionMask(tinyxml2::XMLElement* xml, const char* attribName)
{
  XR_ASSERT(UIBuilder, xml != nullptr);
  XR_ASSERT(UIBuilder, attribName != nullptr);

  uint32_t dimensions = 0;
  const char* value = xml->Attribute(attribName);
  if (value != nullptr)
  {
    for (int i = 0; i < kNumXmlDimensions; ++i)
    {
      if (strchr(value, kXmlDimensionChars[i]))
      {
        dimensions |= XR_MASK_ID(decltype(dimensions), i);
      }
    }
  }
  return dimensions;
}

//==============================================================================
void UIBSetUISprite(const Sprite* sprite, Sprite& outSprite, UIElement* uiElem)
{
  XR_ASSERT(UIBuilder, sprite != nullptr);
  outSprite = *sprite;

  float hw(sprite->GetHalfWidth());
  if (uiElem->w == 0)
  {
    uiElem->w = static_cast<int32_t>(hw * 2.0f);
  }
  else if (hw > .0f)
  {
    outSprite.ScaleX((uiElem->w / 2) / hw);
  }

  float hh(sprite->GetHalfHeight());
  if (uiElem->h == 0)
  {
    uiElem->h = static_cast<int32_t>(hh * 2.0f);
  }
  else if (sprite->GetHalfHeight() > .0f)
  {
    outSprite.ScaleY((uiElem->h / 2) / hh);
  }
}

//==============================================================================
// creator callbacks
// spacer
UIElement*  UIBCreateUISpacer(AllocateCallback allocCb,
  void* allocCbData)
{
  void* buffer = (*allocCb)(sizeof(UISpacer), allocCbData);
  return new (static_cast<UISpacer*>(buffer)) UISpacer;
}

//==============================================================================
bool  UIBInitUIElement(tinyxml2::XMLElement* xml, UIElement* uiElem,
        UIContainer* parent, const UIBuilder& builder)
{
  bool  success = true;

  int temp;
  // x
  GetXmlIntAttribute(xml, "x", Gfx::GetLogicalWidth(), temp);

  success = CheckInt16(temp);
  if (!success)
  {
    XR_TRACE(UIBuilder, ("UIElement requires a 16 bit 'x' value (got %d).", temp));
  }

  if (success)
  {
    uiElem->x = temp;
  }

  // y
  if (success)
  {
    GetXmlIntAttribute(xml, "y", Gfx::GetLogicalHeight(), temp);

    success = CheckInt16(temp);
    if (!success)
    {
      XR_TRACE(UIBuilder, ("UIElement requires a 16 bit 'y' value (got %d).", temp));
    }
  }

  if (success)
  {
    uiElem->y = temp;
  }

  // w
  if (success)
  {
    GetXmlIntAttribute(xml, "w", parent->w, temp);

    success = CheckInt16(temp);
    if (!success)
    {
      XR_TRACE(UIBuilder, ("UIElement requires a 16 bit 'w' value (got %d).", temp));
    }
  }

  if (success)
  {
    uiElem->w = temp;
  }

  // h
  if (success)
  {
    GetXmlIntAttribute(xml, "h", parent->h, temp);
    success = CheckInt16(temp);
    if (!success)
    {
      XR_TRACE(UIBuilder, ("UIElement requires a 16 bit 'h' value (got %d).", temp));
    }
  }

  if (success)
  {
    uiElem->h = temp;
  }

  return success;
}

//==============================================================================
// colored elements
bool  UIBInitUIColoredElement(tinyxml2::XMLElement* xml, UIElement* uiElem,
        UIContainer* parent, const UIBuilder& builder)
{
  bool  success = UIBInitUIElement(xml, uiElem, parent, builder);
  if (success)
  {
    UIColoredElement* coloredElem = static_cast<UIColoredElement*>(uiElem);

    const char* value(xml->Attribute("color"));
    if (value != nullptr)
    {
      uint32_t  color(strtoul(value, 0, 16));
      if (strlen(value) <= 6)
      {
        color |= 0xff000000;
      }
      color = (color & 0xff00ff00) | ((color & 0xff0000) >> 16) | ((color & 0xff) << 16);
      coloredElem->color.Set(color);
    }
  }
  return success;
}

//==============================================================================
// label
UIElement*  UIBCreateUILabel(AllocateCallback allocCb, void* allocCbData)
{
  void* buffer = (*allocCb)(sizeof(UILabel), allocCbData);
  return new (static_cast<UILabel*>(buffer)) UILabel;
}

//==============================================================================
bool  UIBInitUILabel(tinyxml2::XMLElement* xml, UIElement* uiElem, UIContainer* parent,
        const UIBuilder& builder)
{
  bool  success = UIBInitUIColoredElement(xml, uiElem, parent, builder);
  if (success)
  {
    UILabel*  label = static_cast<UILabel*>(uiElem);

    const char* value = nullptr;

    // font
    value = xml->Attribute("font");
    if (value != nullptr)
    {
      Font::Ptr font = Asset::Manager::Find<Font>(value);
      success = font;
      if (!success)
      {
        XR_TRACE(UIBuilder, ("Couldn't find font '%s' for UILabel.", value));
      }

      if (success)
      {
        label->SetFont(font);
      }
    }

    // horizontal alignment
    value = xml->Attribute("hAlign");
    if (value != nullptr)
    {
      const uint32_t* pFind = std::find(kHorizontalAlignNameHashes,
        kHorizontalAlignNameHashes + ARRAY_SIZE(kHorizontalAlignNameHashes),
        Hash::String32(value));

      int value = static_cast<int32_t>(pFind - kHorizontalAlignNameHashes);
      if (value < ARRAY_SIZE(kHorizontalAlignNameHashes))
      {
        label->SetHorizontalAlignment(static_cast<BoxText::Alignment>(value));
      }
    }

    // vertical alignment
    value = xml->Attribute("vAlign");
    if (value != nullptr)
    {
      const uint32_t* pFind = std::find(kVerticalAlignNameHashes, kVerticalAlignNameHashes +
        ARRAY_SIZE(kVerticalAlignNameHashes), Hash::String32(value));

      int value = static_cast<int32_t>(pFind - kVerticalAlignNameHashes);
      if (value < ARRAY_SIZE(kVerticalAlignNameHashes))
      {
        label->SetVerticalAlignment(static_cast<BoxText::Alignment>(value));
      }
    }

    // text
    tinyxml2::XMLNode* node = xml->FirstChild();
    while (node != nullptr)
    {
      tinyxml2::XMLText* xmlText = node->ToText();
      if (xmlText)
      {
        const char* text = xmlText->Value();
        if (text != nullptr)
        {
          success = label->GetFont();
          if (!success)
          {
            const int kBufferSize = 32;
            char  buffer[kBufferSize + 1];
            snprintf(buffer, kBufferSize, "%s", text);

            XR_TRACE(UIBuilder, ("A font needs to be set on UILabel to prepare text '%s'.", buffer));
          }

          if (success)
          {
            label->SetText(builder.ProcessText(text).c_str());
          }
          break;
        }
      }
      node = node->NextSibling();
    }

    // size to content
    uint32_t sizeToContentValue = GetXmlDimensionMask(xml, "sizeToContent");
    switch (sizeToContentValue)
    {
    case XR_MASK_ID(uint32_t, XD_WIDTH) | XR_MASK_ID(uint32_t, XD_HEIGHT):
      label->SetSizeToText();
      break;

    case XR_MASK_ID(uint32_t, XD_WIDTH):
      label->SetWidthToText();
      break;

    case XR_MASK_ID(uint32_t, XD_HEIGHT):
      label->SetHeightToText();
      break;
    }
  }
  return success;
}

//==============================================================================
// image
UIElement*  UIBCreateUIImage(AllocateCallback allocCb, void* allocCbData)
{
  void* buffer = (*allocCb)(sizeof(UIImage), allocCbData);
  return new (static_cast<UIImage*>(buffer)) UIImage;
}

//==============================================================================
bool  UIBInitUIImage(tinyxml2::XMLElement* xml, UIElement* uiElem, UIContainer* parent,
        const UIBuilder& builder)
{
  bool  success = UIBInitUIColoredElement(xml, uiElem, parent, builder);
  if (success)
  {
    UIImage*  image = static_cast<UIImage*>(uiElem);

    // scale
    float scale = GetXmlScaleAttribute(xml);

    // material
    const char* value = nullptr;
    value = xml->Attribute("img");
    success = value != nullptr;
    if (!success)
    {
      XR_TRACE(UIBuilder, ("UIImage requires an 'img' attribute."));
    }

    if (success)
    {
      const Sprite* sprite = builder.GetSprite(value);
      success = sprite != nullptr;
      if (!success)
      {
        XR_TRACE(UIBuilder, ("Couldn't find sprite '%s' for UIImage.", value));
      }

      if (success)
      {
        UIBSetUISprite(sprite, image->sprite, image);
        image->sprite.Scale(scale);
      }
    }
  }

  return success;
}

//==============================================================================
// imagepanel
UIElement*  UIBCreateUIImagePanel(AllocateCallback allocCb, void* allocCbData)
{
  void* buffer = (*allocCb)(sizeof(UIImagePanel), allocCbData);
  return new (static_cast<UIImagePanel*>(buffer)) UIImagePanel;
}

//==============================================================================
bool  UIBInitUIImagePanel(tinyxml2::XMLElement* xml, UIElement* uiElem,
        UIContainer* parent, const UIBuilder& builder)
{
  bool  success = UIBInitUIColoredElement(xml, uiElem, parent, builder);
  if (success)
  {
    UIImagePanel*  imagePanel = static_cast<UIImagePanel*>(uiElem);

    // scale
    float scale = GetXmlScaleAttribute(xml);

    // material
    const char* value = nullptr;
    value = xml->Attribute("img");
    success = value != nullptr;
    if (!success)
    {
      XR_TRACE(UIBuilder, ("UIImagePanel requires an 'img' attribute."));
    }

    if (success)
    {
      const Sprite* sprite = builder.GetSprite(value);
      success = sprite != nullptr;
      if (!success)
      {
        XR_TRACE(UIBuilder, ("Couldn't find sprite '%s' for UIImagePanel.", value));
      }

      if (success)
      {
        imagePanel->sprite = *sprite;
      }
    }

    if (success)
    {
      imagePanel->sprite.Scale(scale);

      float temp;
      if (GetXmlFloatAttribute(xml, "hSplit", temp))
      {
        imagePanel->hSplit = temp;
      }

      if (GetXmlFloatAttribute(xml, "vSplit", temp))
      {
        imagePanel->vSplit = temp;
      }
    }
  }
  return success;
}

//==============================================================================
// progress bars
UIElement*  UIBCreateUIHorizontalProgressBar(AllocateCallback allocCb,
              void* allocCbData)
{
  void* buffer = (*allocCb)(sizeof(UIHorizontalProgressBar), allocCbData);
  return new (static_cast<UIHorizontalProgressBar*>(buffer))
    UIHorizontalProgressBar;
}

//==============================================================================
UIElement*  UIBCreateUIVerticalProgressBar(AllocateCallback allocCb,
              void* allocCbData)
{
  void* buffer = (*allocCb)(sizeof(UIVerticalProgressBar), allocCbData);
  return new (static_cast<UIVerticalProgressBar*>(buffer)) UIVerticalProgressBar;
}

//==============================================================================
bool  UIBInitUIProgressBarBase(tinyxml2::XMLElement* xml, UIElement* uiElem,
        UIContainer* parent, const UIBuilder& builder)
{
  bool  success = UIBInitUIImage(xml, uiElem, parent, builder);
  if (success)
  {
    UIProgressBarBase*  progressBar = static_cast<UIProgressBarBase*>(uiElem);

    // fill direction
    if (success)
    {
      const char* value = xml->Attribute("fillDir");
      if (value != nullptr)
      {
        const uint32_t* pFind = std::find(kDirectionNameHashes, kDirectionNameHashes +
          ARRAY_SIZE(kDirectionNameHashes), Hash::String32(value));

        int value = static_cast<int32_t>(pFind - kDirectionNameHashes);
        success = value < ARRAY_SIZE(kDirectionNameHashes);
        if (!success)
        {
          XR_TRACE(UIBuilder, ("Invalid value in 'fillDir' element for UIProgressBarBase: %s",
            value));
        }

        if (success)
        {
          progressBar->SetFillDirection(static_cast<UIProgressBarBase::FillDir>(value));
        }
      }

      // percent
      float percent;
      GetXmlFloatAttribute(xml, "percent", percent);
      progressBar->SetPercentage(percent);
    }
  }
  return success;
}

//==============================================================================
// sliders
bool  UIBInitUISliderBase(tinyxml2::XMLElement* xml, UIElement* uiElem,
        UIContainer* parent, const UIBuilder& builder)
{
  bool  success = UIBInitUIImage(xml, uiElem, parent, builder);
  if (success)
  {
    UISliderBase* slider = static_cast<UISliderBase*>(uiElem);

    // scale
    float scale = GetXmlScaleAttribute(xml);

    const char* value = nullptr;
    // material
    if (success)
    {
      value = xml->Attribute("imgSlider");

      success = value != nullptr;
      if (!success)
      {
        XR_TRACE(UIBuilder,
          ("'imgSlider' attribute needs to be set for UISliderBase."));
      }

      if (success)
      {
        const Sprite* sprite = builder.GetSprite(value);
        success = sprite != nullptr;
        if (!success)
        {
          XR_TRACE(UIBuilder, ("Couldn't find Sprite '%s' for UISliderBase.", value));
        }

        if (success)
        {
          slider->sliderSprite = *sprite;
          slider->sprite.Scale(scale); // might suck
        }
      }
    }
  }
  return success;
}

//==============================================================================
UIElement*  UIBCreateUIVerticalSlider(AllocateCallback allocCb,
              void* allocCbData)
{
  return new (static_cast<UIVerticalSlider*>((*allocCb)(sizeof(UIVerticalSlider),
    allocCbData))) UIVerticalSlider;
}

//==============================================================================
UIElement*  UIBCreateUIHorizontalSlider(AllocateCallback allocCb,
              void* allocCbData)
{
  return new (static_cast<UIHorizontalSlider*>((*allocCb)(sizeof(UIHorizontalSlider),
    allocCbData))) UIHorizontalSlider;
}

//==============================================================================
// button
UIElement*  UIBCreateUIButton(AllocateCallback allocCb, void* allocCbData)
{
  return new (static_cast<UIButton*>((*allocCb)(sizeof(UIButton),
    allocCbData))) UIButton;
}

//==============================================================================
bool  UIBInitUIButton(tinyxml2::XMLElement* xml, UIElement* uiElem,
        UIContainer* parent, const UIBuilder& builder)
{
  bool  success = UIBInitUIColoredElement(xml, uiElem, parent, builder);
  if (success)
  {
    UIButton* button = static_cast<UIButton*>(uiElem);

    // enabled -- optional
    const char* value = nullptr;
    value = xml->Attribute("enabled");
    if (!Parse::BooleanPositive(value))
    {
      button->SetEnabled(false);
    }

    // scale -- optional
    float scale = GetXmlScaleAttribute(xml);

    // material
    bool  isSet[UIButton::kNumStates] = { false, false, false };

    if (success)
    {
      value = xml->Attribute("img");
      if (value == 0)
      {
        value = xml->Attribute("imgUp");
      }

      success = value != nullptr;
      if (!success)
      {
        XR_TRACE(UIBuilder,
          ("'img' or 'imgUp' attribute needs to be set for UIButton."));
      }

      if (success)
      {
        const Sprite*  sprite = builder.GetSprite(value);
        success = sprite != nullptr;
        if (!success)
        {
          XR_TRACE(UIBuilder, ("Couldn't find Sprite '%s' for img{Up}.", value));
        }

        if (success)
        {
          isSet[UIButton::S_UP] = success;

          Sprite& sprOut = button->sprites[UIButton::S_UP];
          UIBSetUISprite(sprite, sprOut, button);
          sprOut.Scale(scale);
        }
      }
    }

    if (success)
    {
      value = xml->Attribute("imgDown");
      if (value != nullptr)
      {
        const Sprite*  sprite = builder.GetSprite(value);
        success = sprite != nullptr;
        if (!success)
        {
          XR_TRACE(UIBuilder, ("Couldn't find Sprite '%s' for imgDown.", value));
        }

        if (success)
        {
          isSet[UIButton::S_DOWN] = success;

          Sprite& sprOut = button->sprites[UIButton::S_DOWN];
          UIBSetUISprite(sprite, sprOut, button);
          sprOut.Scale(scale);
        }
      }
    }

    if (success)
    {
      value = xml->Attribute("imgOff");
      if (value != nullptr)
      {
        const Sprite*  sprite = builder.GetSprite(value);
        success = sprite != nullptr;
        if (!success)
        {
          XR_TRACE(UIBuilder, ("Couldn't find Sprite '%s' for imgOff.", value));
        }

        if (success)
        {
          isSet[UIButton::S_OFF] = success;

          Sprite& sprOut = button->sprites[UIButton::S_OFF];
          UIBSetUISprite(sprite, sprOut, button);
          sprOut.Scale(scale);
        }
      }
    }

    if (success)
    {
      if (!isSet[UIButton::S_DOWN])
      {
        button->sprites[UIButton::S_DOWN] = button->sprites[UIButton::S_UP];
      }

      if (!isSet[UIButton::S_OFF])
      {
        button->sprites[UIButton::S_OFF] = button->sprites[UIButton::S_DOWN];
      }
    }
  }
  return success;
}

//==============================================================================
// checkbox
UIElement*  UIBCreateUICheckBox(AllocateCallback allocCb, void* allocCbData)
{
  return new (static_cast<UICheckBox*>((*allocCb)(sizeof(UICheckBox),
    allocCbData))) UICheckBox;
}

//==============================================================================
bool  UIBInitUICheckBox(tinyxml2::XMLElement* xml, UIElement* uiElem,
        UIContainer* parent, const UIBuilder& builder)
{
  bool  success = UIBInitUIButton(xml, uiElem, parent, builder);
  if (success)
  {
    UICheckBox* checkBox = static_cast<UICheckBox*>(uiElem);

    // selected -- optional
    const char* value = nullptr;
    value = xml->Attribute("selected");
    if (!Parse::BooleanNegative(value))
    {
      checkBox->SetSelected(false);
    }

    // material
    if (success)
    {
      value = xml->Attribute("imgSet");
      if (value != nullptr)
      {
        const Sprite*  sprite = builder.GetSprite(value);
        success = sprite != nullptr;
        if (!success)
        {
          XR_TRACE(UIBuilder, ("Couldn't find Sprite '%s' for imgSet.", value));
        }

        if (success)
        {
          UIBSetUISprite(sprite, checkBox->setSprite, checkBox);
        }
      }
    }

  }
  return success;
}

//==============================================================================
// radio button
UIElement*  UIBCreateUIRadioButton(AllocateCallback allocCb,
              void* allocCbData)
{
  void* buffer = (*allocCb)(sizeof(UIRadioButton), allocCbData);
  return new (static_cast<UIRadioButton*>(buffer)) UIRadioButton;
}

//==============================================================================
bool  UIBInitUIRadioButton(tinyxml2::XMLElement* xml, UIElement* uiElem,
        UIContainer* parent, const UIBuilder& builder)
{
  bool  success = UIBInitUICheckBox(xml, uiElem, parent, builder);
  if (success)
  {
    UIRadioButton*  pRadioButton(static_cast<UIRadioButton*>(uiElem));

    const char* value(xml->Attribute("group"));
    if (value != nullptr && strlen(value) > 0)
    {
      pRadioButton->SetGroup(value);
    }
  }
  return success;
}

//==============================================================================
// aligner
UIElement*  UIBCreateUIAligner(AllocateCallback allocCb, void* allocCbData)
{
  void* buffer = (*allocCb)(sizeof(UIAligner), allocCbData);
  return new (static_cast<UIAligner*>(buffer)) UIAligner;
}

//==============================================================================
bool  UIBInitUIAligner(tinyxml2::XMLElement* xml, UIElement* uiElem,
        UIContainer* parent, const UIBuilder& builder)
{
  bool  success = UIBInitUIElement(xml, uiElem, parent, builder);
  if (success)
  {
    UIAligner*  pAligner(static_cast<UIAligner*>(uiElem));

    // horizontal alignment
    int value;
    value = UIBuilder::GetXmlAlignment(xml, "hAlign");
    if (value < UIBuilder::XA_NONE)
    {
      pAligner->SetHorizontalAlignment(static_cast<UIElement::Alignment>(value - 1));
    }

    // vertical alignment
    value = UIBuilder::GetXmlAlignment(xml, "vAlign");
    if (value < UIBuilder::XA_NONE)
    {
      pAligner->SetVerticalAlignment(static_cast<UIElement::Alignment>(value - 1));
    }
  }
  return success;
}

//==============================================================================
// cascader
UIElement*  UIBCreateUICascader(AllocateCallback allocCb, void* allocCbData)
{
  void* buffer = (*allocCb)(sizeof(UICascader), allocCbData);
  return new (static_cast<UICascader*>(buffer)) UICascader;
}

//==============================================================================
bool  UIBInitUICascader(tinyxml2::XMLElement* xml, UIElement* uiElem,
        UIContainer* parent, const UIBuilder& builder)
{
  bool  success = UIBInitUIAligner(xml, uiElem, parent, builder);
  if (success)
  {
    UICascader* pCascader(static_cast<UICascader*>(uiElem));

    int temp;
    GetXmlIntAttribute(xml, "hOffset", pCascader->w, temp);
    success = CheckInt16(temp);
    if (success)
    {
      pCascader->SetHorizontalOffset(temp);
    }
    else
    {
      XR_TRACE(UIBuilder,
        ("UICascader requires a 16 bit 'hOffset' value (got %d).", temp));
    }

    if (success)
    {
      GetXmlIntAttribute(xml, "vOffset", pCascader->h, temp);
      success = CheckInt16(temp);
      if (!success)
      {
        XR_TRACE(UIBuilder,
          ("UICascader requires a 16 bit 'vOffset' value (got %d).", temp));
      }
    }

    if (success)
    {
      pCascader->SetVerticalOffset(temp);
    }
  }
  return success;
}

//==============================================================================
// growing layouts
bool  UIBInitUIGrowingLayout(tinyxml2::XMLElement* xml, UIElement* uiElem,
        UIContainer* parent, const UIBuilder& builder)
{
  bool  success = UIBInitUIElement(xml, uiElem, parent, builder);
  if (success)
  {
    UIGrowingLayoutBase*  pLayout(static_cast<UIGrowingLayoutBase*>(uiElem));

    const char* value = nullptr;

    // alignment
    int intValue = UIBuilder::GetXmlAlignment(xml, "align");
    if (intValue < UIBuilder::XA_NONE)
    {
      pLayout->SetAlignment(static_cast<UIElement::Alignment>(intValue - 1));
    }

    // grow direction
    value = xml->Attribute("growDir");
    if (value != nullptr)
    {
      const uint32_t* pFind = std::find(kDirectionNameHashes, kDirectionNameHashes +
        ARRAY_SIZE(kDirectionNameHashes), Hash::String32(value));

      int value = static_cast<int32_t>(pFind - kDirectionNameHashes);
      success = value < ARRAY_SIZE(kDirectionNameHashes);
      if (success)
      {
        pLayout->SetGrowDir(static_cast<UIGrowingLayoutBase::GrowDir>(value));
      }
      else
      {
        XR_TRACE(UIBuilder,
          ("Invalid value in 'growDir' element for UIGrowingLayoutBase: %s",
            value));
      }
    }

    // spacing
    int spacing = 0;
    if (success)
    {
      GetXmlIntAttribute(xml, "spacing",
        UIGrowingLayoutBase::GetDefaultSpacing(), spacing);
      success = CheckInt16(spacing);

      if (!success)
      {
        XR_TRACE(UIBuilder,
          ("UIGrowingLayoutBase requires a 16 bit 'spacing' value (got %d).",
            spacing));
      }
    }

    if (success)
    {
      pLayout->SetSpacing(spacing);
    }
  }
  return success;
}

//==============================================================================
UIElement*  UIBCreateUIHorizontalLayout(AllocateCallback allocCb,
              void* allocCbData)
{
  return new (static_cast<UIHorizontalLayout*>((*allocCb)(sizeof(UIHorizontalLayout),
    allocCbData))) UIHorizontalLayout;
}

//==============================================================================
UIElement*  UIBCreateUIVerticalLayout(AllocateCallback allocCb,
              void* allocCbData)
{
  return new (static_cast<UIVerticalLayout*>((*allocCb)(sizeof(UIVerticalLayout),
    allocCbData))) UIVerticalLayout;
}

//==============================================================================
UIElement*  UIBCreateUIHorizontalScrollingLayout(AllocateCallback allocCb,
              void* allocCbData)
{
  return new (static_cast<UIHorizontalScrollingLayout*>((*allocCb)(sizeof(UIVerticalScrollingLayout),
    allocCbData))) UIHorizontalScrollingLayout;
}

//==============================================================================
bool  UIBInitUIHorizontalScrollingLayout(tinyxml2::XMLElement* xml, UIElement* uiElem,
        UIContainer* parent, const UIBuilder& builder)
{
  bool  success = UIBInitUIGrowingLayout(xml, uiElem, parent, builder);
  if(success)
  {
    UIHorizontalScrollingLayout*  pLayout(static_cast<UIHorizontalScrollingLayout*>(uiElem));

    int xOffset;
    if (GetXmlIntAttribute(xml, "xOffset", pLayout->w, xOffset))
    {
      pLayout->SetXOffset(xOffset);
    }

    float sensitivity;
    if (GetXmlFloatAttribute(xml, "sensitivity", sensitivity))
    {
      pLayout->SetSensitivity(sensitivity);
    }
  }

  return success;
}

//==============================================================================
UIElement*  UIBCreateUIVerticalScrollingLayout(AllocateCallback allocCb,
              void* allocCbData)
{
  return new (static_cast<UIVerticalScrollingLayout*>((*allocCb)(sizeof(UIVerticalScrollingLayout),
    allocCbData))) UIVerticalScrollingLayout;
}

//==============================================================================
bool  UIBInitUIVerticalScrollingLayout(tinyxml2::XMLElement* xml, UIElement* uiElem,
        UIContainer* parent, const UIBuilder& builder)
{
  bool  success = UIBInitUIGrowingLayout(xml, uiElem, parent, builder);
  if(success)
  {
    UIVerticalScrollingLayout*  pLayout(static_cast<UIVerticalScrollingLayout*>(uiElem));

    int yOffset;
    if (GetXmlIntAttribute(xml, "yOffset", pLayout->h, yOffset))
    {
      pLayout->SetYOffset(yOffset);
    }

    float sensitivity;
    if (GetXmlFloatAttribute(xml, "sensitivity", sensitivity))
    {
      pLayout->SetSensitivity(sensitivity);
    }
  }

  return success;
}

//==============================================================================
// grid layout
UIElement*  UIBCreateUIGridLayout(AllocateCallback allocCb,
              void* allocCbData)
{
  return new (static_cast<UIGridLayout*>((*allocCb)(sizeof(UIGridLayout),
    allocCbData))) UIGridLayout;
}

//==============================================================================
bool  UIBInitUIGridLayout(tinyxml2::XMLElement* xml, UIElement* uiElem,
        UIContainer* parent, const UIBuilder& builder)
{
  bool  success = UIBInitUIElement(xml, uiElem, parent, builder);
  if(success)
  {
    UIGridLayout*  pLayout(static_cast<UIGridLayout*>(uiElem));

    int value = 1;

    // rows
    if (success)
    {
      success = xml->QueryIntAttribute("rows", &value) != tinyxml2::XML_WRONG_ATTRIBUTE_TYPE &&
        CheckInt16(value) && value >= 0;

      if (!success)
      {
        XR_TRACE(UIBuilder, ("UIGridLayout requires a positive 16 bit 'rows' value (got %d).",
          value));
      }
    }

    if (success)
    {
      pLayout->SetNumRows(value);
    }

    // columns
    if (success)
    {
      success = xml->QueryIntAttribute("cols", &value) != tinyxml2::XML_WRONG_ATTRIBUTE_TYPE &&
        CheckInt16(value) && value >= 0;

      if (!success)
      {
        XR_TRACE(UIBuilder, ("UIGridLayout requires a positive 16 bit 'cols' value (got %d).",
          value));
      }
    }

    if (success)
    {
      pLayout->SetNumColumns(value);
    }

    // cell width
    if (success && GetXmlIntAttribute(xml, "wCell", parent->w, value))
    {
      success = CheckInt16(value);

      if (!success)
      {
        XR_TRACE(UIBuilder, ("UIGridLayout requires a 16 bit 'wCell' value (got %d).",
          value));
      }

      if (success)
      {
        pLayout->SetCellWidth(value);
      }
    }

    // cell height
    if (success && GetXmlIntAttribute(xml, "hCell", parent->h, value))
    {
      success = CheckInt16(value);

      if (!success)
      {
        XR_TRACE(UIBuilder, ("UIGridLayout requires a 16 bit 'hCell' value (got %d).",
          value));
      }

      if (success)
      {
        pLayout->SetCellWidth(value);
      }
    }

    // column padding
    if (success && GetXmlIntAttribute(xml, "colPadding",
      UIGridLayout::GetDefaultRowPadding(), value))
    {
      success = CheckInt16(value);

      if (!success)
      {
        XR_TRACE(UIBuilder, ("UIGridLayout requires a 16 bit 'colPadding' value (got %d).",
          value));
      }

      if (success)
      {
        pLayout->SetColumnPadding(value);
      }
    }

    // row padding
    if (success && GetXmlIntAttribute(xml, "rowPadding",
      UIGridLayout::GetDefaultRowPadding(), value))
    {
      success = CheckInt16(value);

      if (!success)
      {
        XR_TRACE(UIBuilder, ("UIGridLayout requires a 16 bit 'rowPadding' value (got %d).",
          value));
      }

      if (success)
      {
        pLayout->SetRowPadding(value);
      }
    }

    if (success)
    {
      // horizontal alignment
      value = UIBuilder::GetXmlAlignment(xml, "hAlign");
      if (value < UIBuilder::XA_NONE)
      {
        pLayout->SetHorizontalAlignment(static_cast<UIElement::Alignment>(value - 1));
      }

      // vertical alignment
      value = UIBuilder::GetXmlAlignment(xml, "vAlign");
      if (value < UIBuilder::XA_NONE)
      {
        pLayout->SetVerticalAlignment(static_cast<UIElement::Alignment>(value - 1));
      }
    }
  }

  return success;
}

//==============================================================================
const char* const UIBuilder::kInclude = "include";

const uint32_t  UIBuilder::kIncludeHash = Hash::String32(kInclude);

const char* const UIBuilder::kAlignValues[] =
{
  "low",
  "center",
  "high",
  "none"
};

const uint32_t UIBuilder::kAlignValueHashes[] =
{
  Hash::String32(kAlignValues[XA_LOW]),
  Hash::String32(kAlignValues[XA_CENTER]),
  Hash::String32(kAlignValues[XA_HIGH]),
  Hash::String32(kAlignValues[XA_NONE])
};

const char* const  UIBuilder::kElementNames[] =
{
  "spacer",
  "button",
  "checkBox",
  "radioButton",
  "label",
  "image",
  "imagePanel",
  "hProgress",
  "vProgress",
  "hSlider",
  "vSlider",
  "aligner",
  "cascader",
  "hLayout",
  "vLayout",
  "hScrollLayout",
  "vScrollLayout",
  "gridLayout",
};

const UIBuilder::Configuration  UIBuilder::kDefaultConfig =
{
  16,
  nullptr,
  nullptr,
  NewAllocate,
  nullptr,
  NewDeallocate,
  nullptr
};

//==============================================================================
int UIBuilder::GetXmlAlignment(tinyxml2::XMLElement* xml, const char* attribName)
{
  XR_ASSERT(UIBuilder, xml != nullptr);
  XR_ASSERT(UIBuilder, attribName != nullptr);

  int value = XA_NONE;
  const char* strValue = xml->Attribute(attribName);
  if (strValue)
  {
    value = static_cast<int32_t>(FindItemId(kAlignValueHashes, kNumAlignValues, Hash::String32(strValue)));
    if (value >= kNumAlignValues) // if not found, default to none
    {
      XR_TRACE(UIBuilder, ("The value of '%s' is not a valid alignment.", attribName));
      value = XA_NONE;
    }
  }
  return value;
}

//==============================================================================
UIBuilder::UIBuilder(const Configuration& cfg)
: m_creators(),
  m_cfg(cfg),
  m_root(nullptr),
  m_depth(0),
  m_levels(nullptr),
  m_handles()
{
  RegisterCreator(kElementNames[UI_SPACER], UIBCreateUISpacer,
    UIBInitUIElement, false);
  RegisterCreator(kElementNames[UI_BUTTON], UIBCreateUIButton,
    UIBInitUIButton, false);
  RegisterCreator(kElementNames[UI_CHECKBOX], UIBCreateUICheckBox,
    UIBInitUICheckBox, false);
  RegisterCreator(kElementNames[UI_RADIOBUTTON], UIBCreateUIRadioButton,
    UIBInitUIRadioButton, false);
  RegisterCreator(kElementNames[UI_LABEL], UIBCreateUILabel, UIBInitUILabel,
    false);
  RegisterCreator(kElementNames[UI_IMAGE], UIBCreateUIImage, UIBInitUIImage,
    false);
  RegisterCreator(kElementNames[UI_IMAGEPANEL], UIBCreateUIImagePanel,
    UIBInitUIImagePanel, false);
  RegisterCreator(kElementNames[UI_HPROGRESS], UIBCreateUIHorizontalProgressBar,
    UIBInitUIProgressBarBase, false);
  RegisterCreator(kElementNames[UI_VPROGRESS], UIBCreateUIVerticalProgressBar,
    UIBInitUIProgressBarBase, false);
  RegisterCreator(kElementNames[UI_HSLIDER], UIBCreateUIHorizontalSlider,
    UIBInitUISliderBase, false);
  RegisterCreator(kElementNames[UI_VSLIDER], UIBCreateUIVerticalSlider,
    UIBInitUISliderBase, false);

  // containers
  RegisterCreator(kElementNames[UI_ALIGNER], UIBCreateUIAligner,
    UIBInitUIAligner, true);
  RegisterCreator(kElementNames[UI_CASCADER], UIBCreateUICascader,
    UIBInitUICascader, true);
  RegisterCreator(kElementNames[UI_HLAYOUT], UIBCreateUIHorizontalLayout,
    UIBInitUIGrowingLayout, true);
  RegisterCreator(kElementNames[UI_VLAYOUT], UIBCreateUIVerticalLayout,
    UIBInitUIGrowingLayout, true);
  RegisterCreator(kElementNames[UI_HSCROLLINGLAYOUT],
    UIBCreateUIHorizontalScrollingLayout,
    UIBInitUIHorizontalScrollingLayout, true);
  RegisterCreator(kElementNames[UI_VSCROLLINGLAYOUT],
    UIBCreateUIVerticalScrollingLayout,
    UIBInitUIVerticalScrollingLayout, true);
  RegisterCreator(kElementNames[UI_GRIDLAYOUT], UIBCreateUIGridLayout,
    UIBInitUIGridLayout, true);
}

//==============================================================================
UIBuilder::~UIBuilder()
{
  Destroy();
  delete[]  m_levels;
}

//==============================================================================
void  UIBuilder::SetConfiguration(const Configuration& cfg)
{
  m_cfg = cfg;
  SetMaxDepth(cfg.maxDepth);
}

//==============================================================================
void UIBuilder::SetMaxDepth(int maxDepth)
{
  XR_ASSERT(UIBuilder, maxDepth >= 0);
  Destroy();

  delete[]  m_levels;
  m_levels = new UIElementList[maxDepth];
  m_cfg.maxDepth = maxDepth;
}

//==============================================================================
void UIBuilder::SetGetSpriteCallback(GetSpriteCallback getSprite, void* userData)
{
  m_cfg.getSprite = getSprite;
  m_cfg.getSpriteUserData = userData;
}

//==============================================================================
void  UIBuilder::SetAllocateCallback(AllocateCallback allocate, void* userData)
{
  m_cfg.allocate = allocate;
  m_cfg.allocateUserData  = userData;
}

//==============================================================================
void  UIBuilder::SetDeallocateCallback(DeallocateCallback deallocate, void* userData)
{
  m_cfg.deallocate = deallocate;
  m_cfg.deallocateUserData = userData;
}

//==============================================================================
void* UIBuilder::Allocate(int size)
{
  XR_ASSERTMSG(UIBuilder, m_cfg.allocate != nullptr,
    ("%s callback was not set.", "Allocate"));
  return (*m_cfg.allocate)(size, m_cfg.allocateUserData);
}

//==============================================================================
void  UIBuilder::Deallocate(void* buffer)
{
  XR_ASSERTMSG(UIBuilder, m_cfg.deallocate != nullptr,
    ("%s callback was not set.", "Deallocate"));
  (*m_cfg.deallocate)(buffer, m_cfg.deallocateUserData);
}

//==============================================================================
const Sprite* UIBuilder::GetSprite(const char* name) const
{
  XR_ASSERTMSG(UIBuilder, m_cfg.getSprite != nullptr,
    ("%s callback was not set.", "GetSprite"));
  return (*m_cfg.getSprite)(name, m_cfg.getSpriteUserData);
}

//==============================================================================
std::string UIBuilder::ProcessText(const char* text) const
{
  if(m_cfg.processText != nullptr)
  {
    return (*m_cfg.processText)(text, m_cfg.processTextUserData);
  }
  else
  {
    return text;
  }
}

//==============================================================================
void  UIBuilder::RegisterCreator(const char* name, CreateCallback createCb,
        InitCallback initCb, bool isContainer)
{
  XR_ASSERT(UIBuilder, name != nullptr);
  UICreatorRecord uicr =
  {
    createCb,
    initCb,
    isContainer
#if defined XR_DEBUG
    , name
#endif
  };
  m_creators[Hash::String32(name)] = uicr;
}

//==============================================================================
bool UIBuilder::RegisterNamedElement(const char* name, UIElement* uiElem)
{
  XR_ASSERT(UIBuilder, name != nullptr);
  XR_ASSERT(UIBuilder, uiElem != nullptr);

  uint32_t              hash(Hash::String32(name));
  ElementMap::iterator  iFind(m_handles.find(hash));
  bool                  success(iFind == m_handles.end());
  if (success)
  {
    ElementMap::value_type  vInsert(hash, uiElem);
    m_handles.insert(vInsert);
  }
  else
  {
    success = iFind->first == hash;
  }
  XR_ASSERTMSG(UIBuilder, success, ("The handle '%s' is already taken", name));

  return success;
}

//==============================================================================
bool  UIBuilder::Build(tinyxml2::XMLElement* xml, UIContainer& container)
{
  XR_ASSERT(IW_ASSERTION_CHANNEL_DEFAULT, xml != nullptr);
  XR_ASSERT(IW_ASSERTION_CHANNEL_DEFAULT, m_cfg.allocate != nullptr);

  m_root = &container;

  UIContainer base(container);
  base.w = Gfx::GetLogicalWidth();
  base.h = Gfx::GetLogicalHeight();

  RegisterNamedElement("root", m_root);
  UIBInitUIElement(xml, m_root, &base, *this);

  int depth(0);
  return _Build(xml, m_root, depth);
}

//==============================================================================
bool  UIBuilder::_Build(tinyxml2::XMLElement* xml, UIContainer* container, int& depth)
{
  XR_ASSERT(UIBuilder, container != nullptr);

  xml = xml->FirstChildElement();
  bool  success(true);
  while (xml)
  {
    UIElement*  uiElem(0);

    uint32_t  hash(Hash::String32(xml->Value()));
    CreatorMap::iterator  iFind(m_creators.find(hash));
    if (iFind != m_creators.end())
    {
      XR_TRACE(UIBuilder, ("Building a %s...", iFind->second.name));

      const UICreatorRecord&  uicr(iFind->second);

      // create element (name)
      uiElem = (*uicr.createCb)(m_cfg.allocate, m_cfg.allocateUserData);
      success = uiElem != nullptr;
      if (!success)
      {
        XR_TRACE(UIBuilder,
          ("Failed to create a %s, possibly due to insufficiend memory.",
            iFind->second.name));
      }

      // setup element (attributes)
      if (success)
      {
        success = (*uicr.initCb)(xml, uiElem, container, *this);
        if (!success)
        {
          XR_TRACE(UIBuilder,
            ("Failed to initialise a %s @ 0x%p. Destruction and deallocation has been attempted. Quitting build.",
              iFind->second.name, uiElem));

          uiElem->~UIElement();
          Deallocate(uiElem);
        }
      }

      if (success)
      {
        // postprocess
        _PostProcess(xml, uiElem);

        // add to parent
        container->AddElement(uiElem);

        m_levels[depth].push_back(uiElem);

        // process container
        if (uicr.isContainer)
        {
          UIContainer*  pMyContainer(static_cast<UIContainer*>(uiElem));

          ++depth;
          if(m_depth < depth)
          {
            m_depth = depth;
          }

          XR_ASSERTMSG(UIBuilder, depth < m_cfg.maxDepth,
            ("maxDepth (%d) too small, try a greater value.", m_cfg.maxDepth));
          success = _Build(xml, pMyContainer, depth);
          _PostProcessContainer(xml, pMyContainer);
          --depth;
        }
      }
    }
    else if (kIncludeHash == hash)
    {
      const char* value(xml->Attribute("src"));
      success = value != nullptr;
      if(success)
      {
        tinyxml2::XMLDocument doc;
        std::string fileName((m_cfg.formatFileName) ?
          (*m_cfg.formatFileName)(value, m_cfg.formatFileNameUserData) :
          value);

        success = doc.LoadFile(fileName.c_str()) &&
          doc.RootElement() != nullptr &&
          _Build(doc.RootElement(), container, depth);
      }
    }

    if (!success)
    {
      break;
    }

    xml = xml->NextSiblingElement();
  }

  return success;
}

//==============================================================================
void UIBuilder::Destroy()
{
  if (m_depth > 0)
  {
    XR_ASSERT(UIBuilder, m_levels != nullptr);

    if (m_root != nullptr)
    {
      UIElementList&  lElems(m_levels[0]);
      for (UIElementList::iterator i0(lElems.begin()), i1(lElems.end());
        i0 != i1; ++i0)
      {
        m_root->RemoveElement(*i0);
      }
    }

    for (int i = 0; i <= m_depth; ++i)
    {
      UIElementList&  lElems(m_levels[i]);
      for (UIElementList::iterator i0(lElems.begin()), i1(lElems.end());
        i0 != i1; ++i0)
      {
        UIElement*  p(*i0);
        p->~UIElement();

        (*m_cfg.deallocate)(p, m_cfg.deallocateUserData);
      }

      m_levels[i].clear();
    }

    m_depth = 0;
  }

  m_handles.clear();
}

//==============================================================================
UIElement* UIBuilder::GetElement(uint32_t hash) const
{
  ElementMap::const_iterator iFind(m_handles.find(hash));
  return (iFind != m_handles.end()) ? iFind->second : 0;
}

//==============================================================================
UIElement* UIBuilder::GetElement(const char* pHandle) const
{
  XR_ASSERT(UIBuilder, pHandle != nullptr);
  return GetElement(Hash::String32(pHandle));
}

//==============================================================================
void UIBuilder::_PostProcess(tinyxml2::XMLElement* xml, UIElement* uiElem)
{
  const char* value(xml->Attribute("handle"));
  if (value != nullptr)
  {
    RegisterNamedElement(value, uiElem);
  }

  const UIElement*  parent(uiElem->GetParent());
  if(parent != nullptr)
  {
    uint32_t  flags(GetXmlDimensionMask(xml, "fillParent"));
    int   w = CheckIdthBit(flags, XD_WIDTH) ? parent->w : uiElem->w;
    int   h = CheckIdthBit(flags, XD_HEIGHT) ? parent->h : uiElem->h;
    uiElem->SetSize(w, h);
  }

  value = xml->Attribute("alignTo");
  if (value != nullptr)
  {
    UIElement*  pAlignTo(GetElement(value));
    XR_ASSERT(UIBuilder, pAlignTo != nullptr);

    int alignTo(XA_NONE);
    int xTarget(0), yTarget(0);

    // horizontal
    alignTo = GetXmlAlignment(xml, "alignToHorizontal");
    if (alignTo == XA_NONE)
    {
      alignTo = XA_LOW;
    }

    xTarget = pAlignTo->x;
    switch (alignTo)
    {
    case XA_LOW:
      break;

    case XA_CENTER:
      xTarget += pAlignTo->w / 2;
      break;

    case XA_HIGH:
      xTarget += pAlignTo->w;
      break;
    }

    int hAlignSide(GetXmlAlignment(xml, "alignSideHorizontal"));
    if (hAlignSide == XA_NONE)
    {
      hAlignSide = alignTo;
    }

    // vertical
    alignTo = GetXmlAlignment(xml, "alignToVertical");
    if (alignTo == XA_NONE)
    {
      alignTo = XA_LOW;
    }

    yTarget = pAlignTo->y;
    switch (alignTo)
    {
    case XA_LOW:
      break;

    case XA_CENTER:
      yTarget += pAlignTo->h / 2;
      break;

    case XA_HIGH:
      yTarget += pAlignTo->h;
      break;
    }

    int vAlignSide(GetXmlAlignment(xml, "alignSideVertical"));
    if (vAlignSide == XA_NONE)
    {
      vAlignSide = alignTo;
    }

    uiElem->Align(xTarget, yTarget, static_cast<UIElement::Alignment>(hAlignSide - 1),
      static_cast<UIElement::Alignment>(vAlignSide - 1));
  }

}

//==============================================================================
void UIBuilder::_PostProcessContainer(tinyxml2::XMLElement* xml, UIContainer* pContainer)
{
  uint32_t sizeToContentValue(GetXmlDimensionMask(xml, "sizeToContent"));
  switch (sizeToContentValue)
  {
  case XR_MASK_ID(uint32_t, XD_WIDTH) | XR_MASK_ID(uint32_t, XD_HEIGHT):
    pContainer->SetSizeToContent();
    break;

  case XR_MASK_ID(uint32_t, XD_WIDTH):
    pContainer->SetWidthToContent();
    break;

  case XR_MASK_ID(uint32_t, XD_HEIGHT):
    pContainer->SetHeightToContent();
    break;
  }
}

//==============================================================================
void* NewAllocate(size_t size, void* userData)
{
  return new Byte[size];
}

//==============================================================================
void  NewDeallocate(void* buffer, void* userData)
{
  delete [] static_cast<Byte*>(buffer);
}

} // xr
