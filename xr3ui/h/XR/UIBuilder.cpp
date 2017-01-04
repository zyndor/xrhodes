#include <XR/Hash.hpp>
#include <XR/Parse.hpp>
#include <XR/Pool.hpp>
#include <XR/utils.hpp>
#include "UIBuilder.hpp"
#include "UISpacer.hpp"
#include "UIRadioButton.hpp"
#include "UILabel.hpp"
#include "UIImagePanel.hpp"
#include "UIHorizontalProgressBar.hpp"
#include "UIVerticalProgressBar.hpp"
#include "UIHorizontalSlider.hpp"
#include "UIVerticalSlider.hpp"
#include "UIAligner.hpp"
#include "UICascader.hpp"
#include "UIHorizontalScrollingLayout.hpp"
#include "UIVerticalScrollingLayout.hpp"
#include "UIGridLayout.hpp"

namespace XR
{

//==============================================================================
static const uint32 karDirectionNameHash[] =
{
  Hash::String("positive"),
  Hash::String("negative")
};

static const uint32 karHorizontalAlignNameHash[] =
{
  Hash::String("left"),
  Hash::String("center"),
  Hash::String("right"),
  //Hash::String("paragraph")
};

static const uint32 karVerticalAlignNameHash[] =
{
  Hash::String("top"),
  Hash::String("middle"),
  Hash::String("bottom")
};

const int kTinyXmlTextType = 4; // TiXmlNode::TINYXML_TEXT or TiXmlNode::TEXT

//==============================================================================
float GetXmlScaleAttribute(TiXmlElement* pXml)
{
  XR_ASSERT(UIBuilder, pXml != 0);
  double  scale;
  pXml->Attribute("scale", &scale);
  if (scale == .0)
  {
    scale = 1.0;
  }
  return static_cast<float>(scale);
}

//==============================================================================
bool  GetXmlIntAttribute(TiXmlElement* pXml, const char* pAttribName, int base,
        int& v)
{
  XR_ASSERT(UIBuilder, pXml != 0);
  XR_ASSERT(UIBuilder, pAttribName != 0);

  const char* pValue(pXml->Attribute(pAttribName));
  return Parse::Int(base, pValue, v);
}

//==============================================================================
bool  GetXmlFloatAttribute(TiXmlElement* pXml, const char* pAttribName, float& v)
{
  XR_ASSERT(UIBuilder, pXml != 0);
  XR_ASSERT(UIBuilder, pAttribName != 0);

  const char* pValue(pXml->Attribute(pAttribName));
  return Parse::Float(pValue, v);
}

//==============================================================================
// size to content values
enum  XmlDimension
{
  XD_WIDTH,
  XD_HEIGHT,
  kNumXmlDimensions
};

static const int karXmlDimensionChars[kNumXmlDimensions] =
{
  'w',
  'h'
};

uint32  GetXmlDimensionMask(TiXmlElement* pXml, const char* pAttribName)
{
  XR_ASSERT(UIBuilder, pXml != 0);
  XR_ASSERT(UIBuilder, pAttribName != 0);

  uint32 value(0);
  const char* pValue(pXml->Attribute(pAttribName));
  if (pValue != 0)
  {
    for (int i = 0; i < kNumXmlDimensions; ++i)
    {
      if (strchr(pValue, karXmlDimensionChars[i]) != 0)
      {
        value |= XR_MASK_ID(i);
      }
    }
  }
  return value;
}

//==============================================================================
void UIBSetUISprite(const Sprite* pSprite, Sprite& outSprite, UIElement* pUIElem) 
{
  XR_ASSERT(UIBuilder, pSprite != 0);
  outSprite = *pSprite;

  float hw(pSprite->GetHalfWidth());
  if (pUIElem->w == 0)
  {
    pUIElem->w = hw * 2.0f;
  }
  else if (hw > .0f)
  {
    outSprite.ScaleX((pUIElem->w / 2) / hw);
  }

  float hh(pSprite->GetHalfHeight());
  if (pUIElem->h == 0)
  {
    pUIElem->h = hh * 2.0f;
  }
  else if (pSprite->GetHalfHeight() > .0f)
  {
    outSprite.ScaleY((pUIElem->h / 2) / hh);
  }
}

//==============================================================================
// creator callbacks
// spacer
UIElement*  UIBCreateUISpacer(AllocateCallback pAllocCb,
  void* pAllocCbData)
{
  void* pMem((*pAllocCb)(sizeof(UISpacer), pAllocCbData));
  return new (static_cast<UISpacer*>(pMem)) UISpacer;
}

//==============================================================================
bool  UIBInitUIElement(TiXmlElement* pXml, UIElement* pUIElem,
        UIContainer* pParent, const UIBuilder& builder)
{
  bool  success(true);

  int temp;
  // x
  if (success)
  {
    GetXmlIntAttribute(pXml, "x", Renderer::GetScreenWidth(), temp);

    success = CheckInt16(temp);
    if (!success)
    {
      XR_TRACE(UIBuilder, ("UIElement requires a 16 bit 'x' value (got %d).", temp));
    }
  }

  if (success)
  {
    pUIElem->x = temp;
  }

  // y
  if (success)
  {
    GetXmlIntAttribute(pXml, "y", Renderer::GetScreenHeight(), temp);

    success = CheckInt16(temp);
    if (!success)
    {
      XR_TRACE(UIBuilder, ("UIElement requires a 16 bit 'y' value (got %d).", temp));
    }  
  }

  if (success)
  {
    pUIElem->y = temp;
  }

  // w
  if (success)
  {
    GetXmlIntAttribute(pXml, "w", pParent->w, temp);

    success = CheckInt16(temp);
    if (!success)
    {
      XR_TRACE(UIBuilder, ("UIElement requires a 16 bit 'w' value (got %d).", temp));
    }  
  }

  if (success)
  {
    pUIElem->w = temp;
  }

  // h
  if (success)
  {
    GetXmlIntAttribute(pXml, "h", pParent->h, temp);
    success = CheckInt16(temp);
    if (!success)
    {
      XR_TRACE(UIBuilder, ("UIElement requires a 16 bit 'h' value (got %d).", temp));
    }  
  }

  if (success)
  {
    pUIElem->h = temp;
  }

  return success;
}

//==============================================================================
// colored elements
bool  UIBInitUIColoredElement(TiXmlElement* pXml, UIElement* pUIElem, 
        UIContainer* pParent, const UIBuilder& builder)
{
  bool  success(UIBInitUIElement(pXml, pUIElem, pParent, builder));
  if (success)
  {
    UIColoredElement* pColElem(static_cast<UIColoredElement*>(pUIElem));

    const char* pValue(pXml->Attribute("color"));
    if (pValue != 0)
    {
      uint32  color(strtoul(pValue, 0, 16));
      if (strlen(pValue) <= 6)
      {
        color |= 0xff000000;
      }
      color = (color & 0xff00ff00) | ((color & 0xff0000) >> 16) | ((color & 0xff) << 16);
      pColElem->color.Set(color);
    }
  }
  return success;
}

//==============================================================================
// label
UIElement*  UIBCreateUILabel(AllocateCallback pAllocCb, void* pAllocCbData)
{
  void* pMem((*pAllocCb)(sizeof(UILabel), pAllocCbData));
  return new (static_cast<UILabel*>(pMem)) UILabel;
}

//==============================================================================
bool  UIBInitUILabel(TiXmlElement* pXml, UIElement* pUIElem, UIContainer* pParent,
        const UIBuilder& builder)
{
  bool  success(UIBInitUIColoredElement(pXml, pUIElem, pParent, builder));
  if (success)
  {
    UILabel*  pLabel(static_cast<UILabel*>(pUIElem));

    const char* pValue(0);

    // font
    pValue = pXml->Attribute("font");
    if (pValue != 0)
    {
      const Font* pFont(builder.GetFont(pValue));
      
      success = pFont != 0;
      if (!success)
      {
        XR_TRACE(UIBuilder, ("Couldn't find font '%s' for UILabel.", pValue));
      }

      if (success)
      {
        pLabel->SetFont(pFont);
      }
    }
    
    // horizontal alignment
    pValue = pXml->Attribute("hAlign");
    if (pValue != 0)
    {
      const uint32* pFind(std::find(karHorizontalAlignNameHash, karHorizontalAlignNameHash + 3, Hash::String(pValue)));
        
      int value(pFind - karHorizontalAlignNameHash);
      if (value < 3)
      {
        pLabel->SetHorizontalAlignment(static_cast<Text::HAlign>(value));
      }
    }
    
    // vertical alignment
    pValue = pXml->Attribute("vAlign");
    if (pValue != 0)
    {
      const uint32* pFind(std::find(karVerticalAlignNameHash, karVerticalAlignNameHash + 3, Hash::String(pValue)));

      int value(pFind - karVerticalAlignNameHash);
      if (value < 3)
      {
        pLabel->SetVerticalAlignment(static_cast<Text::VAlign>(value));
  	  }
    }
    
    // text
    TiXmlNode* pNode(pXml->FirstChild());
    while (pNode != 0)
    {
      if (pNode->Type() == kTinyXmlTextType)
      {
        const char* pText(pNode->ToText()->Value());
        if (pText != 0)
        {
          success = pLabel->GetFont() != 0;
          if (!success)
          {
            const int kBufferSize = 32;
            char  arBuffer[kBufferSize + 1];
            snprintf(arBuffer, kBufferSize, "%s", pText);

            XR_TRACE(UIBuilder, ("A font need to be set on UILabel to prepare text '%s'.", arBuffer));
          }

          if (success)
          {
            pLabel->PrepareText(builder.ProcessText(pText).c_str());
          }
          break;
        }
      }
      pNode = pNode->NextSibling();
    }

    // size to content
    uint32 sizeToContentValue(GetXmlDimensionMask(pXml, "sizeToContent"));
    if (IsFullMask(sizeToContentValue, XR_MASK_ID(XD_WIDTH) | XR_MASK_ID(XD_HEIGHT)))
    {
      pLabel->SetSizeToText();
    }
    else
    {
      if (IsFullMask(sizeToContentValue, XR_MASK_ID(XD_WIDTH)))
      {
        pLabel->SetWidthToText();
      }

      if (IsFullMask(sizeToContentValue, XR_MASK_ID(XD_HEIGHT)))
      {   
        pLabel->SetHeightToText();
      }
    }
  }
  return success;
}

//==============================================================================
// image
UIElement*  UIBCreateUIImage(AllocateCallback pAllocCb, void* pAllocCbData)
{
  void* pMem((*pAllocCb)(sizeof(UIImage), pAllocCbData));
  return new (static_cast<UIImage*>(pMem)) UIImage;
}

//==============================================================================
bool  UIBInitUIImage(TiXmlElement* pXml, UIElement* pUIElem, UIContainer* pParent,
        const UIBuilder& builder)
{
  bool  success(UIBInitUIColoredElement(pXml, pUIElem, pParent, builder));
  if (success)
  {
    UIImage*  pImage(static_cast<UIImage*>(pUIElem));

    // scale
    float scale(GetXmlScaleAttribute(pXml));

    // material
    const char* pValue(0);
    pValue = pXml->Attribute("img");
    success = pValue != 0;
    if (!success)
    {
      XR_TRACE(UIBuilder, ("UIImage requires an 'img' attribute."));
    }

    if (success)
    {
      const Sprite* pSprite(builder.GetSprite(pValue));
      success = pSprite != 0;
      if (!success)
      {
        XR_TRACE(UIBuilder, ("Couldn't find sprite '%s' for UIImage.", pValue));
      }

      if (success)
      {
        UIBSetUISprite(pSprite, pImage->sprite, pImage);
        pImage->sprite.Scale(scale);
      }
    }
  }

  return success;
}

//==============================================================================
// imagepanel
UIElement*  UIBCreateUIImagePanel(AllocateCallback pAllocCb, void* pAllocCbData)
{
  void* pMem((*pAllocCb)(sizeof(UIImagePanel), pAllocCbData));
  return new (static_cast<UIImagePanel*>(pMem)) UIImagePanel;
}

//==============================================================================
bool  UIBInitUIImagePanel(TiXmlElement* pXml, UIElement* pUIElem,
        UIContainer* pParent, const UIBuilder& builder)
{
  bool  success(UIBInitUIColoredElement(pXml, pUIElem, pParent, builder));
  if (success)
  {
    UIImagePanel*  pImagePanel(static_cast<UIImagePanel*>(pUIElem));

    // scale
    float scale(GetXmlScaleAttribute(pXml));

    // material
    const char* pValue(0);
    pValue = pXml->Attribute("img");
    success = pValue != 0;
    if (!success)
    {
      XR_TRACE(UIBuilder, ("UIImagePanel requires an 'img' attribute."));
    }

    if (success)
    {
      const Sprite* pSprite(builder.GetSprite(pValue));
      success = pSprite != 0;
      if (!success)
      {
        XR_TRACE(UIBuilder, ("Couldn't find sprite '%s' for UIImagePanel.", pValue));
      }

      if (success)
      {
        pImagePanel->sprite = *pSprite;
      }
    }

    if (success)
    {
      pImagePanel->sprite.Scale(scale);
      
      float temp;
      if (GetXmlFloatAttribute(pXml, "hSplit", temp))
      {
        pImagePanel->hSplit = temp;
      }

      if (GetXmlFloatAttribute(pXml, "vSplit", temp))
      {
        pImagePanel->vSplit = temp;
      }
    }
  }
  return success;
}

//==============================================================================
// progress bars
UIElement*  UIBCreateUIHorizontalProgressBar(AllocateCallback pAllocCb,
              void* pAllocCbData)
{
  void* pMem((*pAllocCb)(sizeof(UIHorizontalProgressBar), pAllocCbData));
  return new (static_cast<UIHorizontalProgressBar*>(pMem))
    UIHorizontalProgressBar;
}

//==============================================================================
UIElement*  UIBCreateUIVerticalProgressBar(AllocateCallback pAllocCb,
              void* pAllocCbData)
{
  void* pMem((*pAllocCb)(sizeof(UIVerticalProgressBar), pAllocCbData));
  return new (static_cast<UIVerticalProgressBar*>(pMem)) UIVerticalProgressBar;
}

//==============================================================================
bool  UIBInitUIProgressBarBase(TiXmlElement* pXml, UIElement* pUIElem,
        UIContainer* pParent, const UIBuilder& builder)
{
  bool  success(UIBInitUIImage(pXml, pUIElem, pParent, builder));
  if (success)
  {
    UIProgressBarBase*  pBar(static_cast<UIProgressBarBase*>(pUIElem));

    // fill direction
    if (success)
    {
      const char* pValue(pXml->Attribute("fillDir"));
      if (pValue != 0)
      {
        const uint32* pFind(std::find(karDirectionNameHash, karDirectionNameHash + 2,
          Hash::String(pValue)));

        int value(pFind - karDirectionNameHash);
        success = value < 2;
        if (!success)
        {
          XR_TRACE(UIBuilder, ("Invalid value in 'fillDir' element for UIProgressBarBase: %s",
            pValue));
        }

        if (success)
        {
          pBar->SetFillDirection(static_cast<UIProgressBarBase::FillDir>(value));
        }
      }

      // percent
      float percent;
      GetXmlFloatAttribute(pXml, "percent", percent);
      pBar->SetPercentage(percent);
    }
  }
  return success;
}

//==============================================================================
// sliders
bool  UIBInitUISliderBase(TiXmlElement* pXml, UIElement* pUIElem,
        UIContainer* pParent, const UIBuilder& builder)
{
  bool  success(UIBInitUIImage(pXml, pUIElem, pParent, builder));
  if (success)
  {
    UISliderBase* pSlider(static_cast<UISliderBase*>(pUIElem));

    // scale
    float scale(GetXmlScaleAttribute(pXml));

    const char* pValue(0);
    // material
    if (success)
    {
      pValue = pXml->Attribute("imgSlider");

      success = pValue != 0;
      if (!success)
      {
        XR_TRACE(UIBuilder,
          ("'imgSlider' attribute needs to be set for UISliderBase."));
      }

      if (success)
      {
        const Sprite* pSprite(builder.GetSprite(pValue));
        success = pSprite != 0;
        if (!success)
        {
          XR_TRACE(UIBuilder, ("Couldn't find Sprite '%s' for UISliderBase.", pValue));
        }

        if (success)
        {
          pSlider->sliderSprite = *pSprite;
          pSlider->sprite.Scale(scale); // might suck
        }
      }
    }
  }
  return success;
}

//==============================================================================
UIElement*  UIBCreateUIVerticalSlider(AllocateCallback pAllocCb,
              void* pAllocCbData)
{
  return new (static_cast<UIVerticalSlider*>((*pAllocCb)(sizeof(UIVerticalSlider),
    pAllocCbData))) UIVerticalSlider;
}

//==============================================================================
UIElement*  UIBCreateUIHorizontalSlider(AllocateCallback pAllocCb,
              void* pAllocCbData)
{
  return new (static_cast<UIHorizontalSlider*>((*pAllocCb)(sizeof(UIHorizontalSlider),
    pAllocCbData))) UIHorizontalSlider;
}

//==============================================================================
// button
UIElement*  UIBCreateUIButton(AllocateCallback pAllocCb, void* pAllocCbData)
{
  return new (static_cast<UIButton*>((*pAllocCb)(sizeof(UIButton),
    pAllocCbData))) UIButton;
}

//==============================================================================
bool  UIBInitUIButton(TiXmlElement* pXml, UIElement* pUIElem,
        UIContainer* pParent, const UIBuilder& builder)
{
  bool  success(UIBInitUIColoredElement(pXml, pUIElem, pParent, builder));
  if (success)
  {
    UIButton* pButton(static_cast<UIButton*>(pUIElem));

    // enabled -- optional
    const char* pValue(0);
    pValue = pXml->Attribute("enabled");
    if (!Parse::BooleanPositive(pValue))
    {
      pButton->SetEnabled(false);
    }

    // scale -- optional
    float scale(GetXmlScaleAttribute(pXml));

    // material
    bool  arIsSet[UIButton::kNumStates] = { false, false, false };
    
    if (success)
    {
      pValue = pXml->Attribute("img");
      if (pValue == 0)
      {
        pValue = pXml->Attribute("imgUp");
      }

      success = pValue != 0;
      if (!success)
      {
        XR_TRACE(UIBuilder,
          ("'img' or 'imgUp' attribute needs to be set for UIButton."));
      }

      if (success)
      {
        const Sprite*  pSprite(builder.GetSprite(pValue));
        success = pSprite != 0;
        if (!success)
        {
          XR_TRACE(UIBuilder, ("Couldn't find Sprite '%s' for img{Up}.", pValue));
        }

        if (success)
        {
          arIsSet[UIButton::S_UP] = success;

          Sprite& sprOut(pButton->arSprite[UIButton::S_UP]);
          UIBSetUISprite(pSprite, sprOut, pButton);
          sprOut.Scale(scale);
        }
      }
    }
    
    if (success)
    {
      pValue = pXml->Attribute("imgDown");
      if (pValue != 0)
      {
        const Sprite*  pSprite(builder.GetSprite(pValue));
        success = pSprite != 0;
        if (!success)
        {
          XR_TRACE(UIBuilder, ("Couldn't find Sprite '%s' for imgDown.", pValue));
        }

        if (success)
        {
          arIsSet[UIButton::S_DOWN] = success;

          Sprite& sprOut(pButton->arSprite[UIButton::S_DOWN]);
          UIBSetUISprite(pSprite, sprOut, pButton);
          sprOut.Scale(scale);
        }
      }
    }

    if (success)
    {
      pValue = pXml->Attribute("imgOff");
      if (pValue != 0)
      {
        const Sprite*  pSprite(builder.GetSprite(pValue));
        success = pSprite != 0;
        if (!success)
        {
          XR_TRACE(UIBuilder, ("Couldn't find Sprite '%s' for imgOff.", pValue));
        }

        if (success)
        {
          arIsSet[UIButton::S_OFF] = success;

          Sprite& sprOut(pButton->arSprite[UIButton::S_OFF]);
          UIBSetUISprite(pSprite, sprOut, pButton);
          sprOut.Scale(scale);
        }
      }
    }

    if (success)
    {
      if (!arIsSet[UIButton::S_DOWN])
      {
        pButton->arSprite[UIButton::S_DOWN] =
          pButton->arSprite[UIButton::S_UP];
      }

      if (!arIsSet[UIButton::S_OFF])
      {
        pButton->arSprite[UIButton::S_OFF] =
          pButton->arSprite[UIButton::S_DOWN];
      }
    }
  }
  return success;
}

//==============================================================================
// checkbox
UIElement*  UIBCreateUICheckBox(AllocateCallback pAllocCb, void* pAllocCbData)
{
  return new (static_cast<UICheckBox*>((*pAllocCb)(sizeof(UICheckBox),
    pAllocCbData))) UICheckBox;
}

//==============================================================================
bool  UIBInitUICheckBox(TiXmlElement* pXml, UIElement* pUIElem,
        UIContainer* pParent, const UIBuilder& builder)
{
  bool  success(UIBInitUIButton(pXml, pUIElem, pParent, builder));
  if (success)
  {
    UICheckBox* pCheckBox(static_cast<UICheckBox*>(pUIElem));

    // selected -- optional
    const char* pValue(0);
    pValue = pXml->Attribute("selected");
    if (!Parse::BooleanNegative(pValue))
    {
      pCheckBox->SetSelected(false);
    }

    // material
    if (success)
    {
      pValue = pXml->Attribute("imgSet");
      if (pValue != 0)
      {
        const Sprite*  pSprite(builder.GetSprite(pValue));
        success = pSprite != 0;
        if (!success)
        {
          XR_TRACE(UIBuilder, ("Couldn't find Sprite '%s' for imgSet.", pValue));
        }

        if (success)
        {
          UIBSetUISprite(pSprite, pCheckBox->setSprite, pCheckBox);
        }
      }
    }

  }
  return success;
}
    
//==============================================================================
// radio button
UIElement*  UIBCreateUIRadioButton(AllocateCallback pAllocCb,
              void* pAllocCbData)
{
  void* pMem((*pAllocCb)(sizeof(UIRadioButton), pAllocCbData));
  return new (static_cast<UIRadioButton*>(pMem)) UIRadioButton;
}
                
//==============================================================================
bool  UIBInitUIRadioButton(TiXmlElement* pXml, UIElement* pUIElem,
        UIContainer* pParent, const UIBuilder& builder)
{
  bool  success(UIBInitUICheckBox(pXml, pUIElem, pParent, builder));
  if (success)
  {
    UIRadioButton*  pRadioButton(static_cast<UIRadioButton*>(pUIElem));
    
    const char* pValue(pXml->Attribute("group"));
    if (pValue != 0 && strlen(pValue) > 0)
    {
      pRadioButton->SetGroup(pValue);
    }
  }
  return success;
}

//==============================================================================
// aligner
UIElement*  UIBCreateUIAligner(AllocateCallback pAllocCb, void* pAllocCbData)
{
  void* pMem((*pAllocCb)(sizeof(UIAligner), pAllocCbData));
  return new (static_cast<UIAligner*>(pMem)) UIAligner;
}

//==============================================================================
bool  UIBInitUIAligner(TiXmlElement* pXml, UIElement* pUIElem,
        UIContainer* pParent, const UIBuilder& builder)
{
  bool  success(UIBInitUIElement(pXml, pUIElem, pParent, builder));
  if (success)
  {
    UIAligner*  pAligner(static_cast<UIAligner*>(pUIElem));

    // horizontal alignment
    int value;
    value = UIBuilder::GetXmlAlignment(pXml, "hAlign");
    if (value < UIBuilder::XA_NONE)
    {
      pAligner->SetHorizontalAlignment(static_cast<UIElement::Alignment>(value - 1));
    }

    // vertical alignment
    value = UIBuilder::GetXmlAlignment(pXml, "vAlign");
    if (value < UIBuilder::XA_NONE)
    {
      pAligner->SetVerticalAlignment(static_cast<UIElement::Alignment>(value - 1));
    }
  }
  return success;
}

//==============================================================================
// cascader
UIElement*  UIBCreateUICascader(AllocateCallback pAllocCb, void* pAllocCbData)
{
  void* pMem((*pAllocCb)(sizeof(UICascader), pAllocCbData));
  return new (static_cast<UICascader*>(pMem)) UICascader;
}

//==============================================================================
bool  UIBInitUICascader(TiXmlElement* pXml, UIElement* pUIElem,
        UIContainer* pParent, const UIBuilder& builder)
{
  bool  success(UIBInitUIAligner(pXml, pUIElem, pParent, builder));
  if (success)
  {
    UICascader* pCascader(static_cast<UICascader*>(pUIElem));
    
    int temp;
    GetXmlIntAttribute(pXml, "hOffset", pCascader->w, temp);
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
      GetXmlIntAttribute(pXml, "vOffset", pCascader->h, temp);
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
bool  UIBInitUIGrowingLayout(TiXmlElement* pXml, UIElement* pUIElem,
        UIContainer* pParent, const UIBuilder& builder)
{
  bool  success(UIBInitUIElement(pXml, pUIElem, pParent, builder));
  if (success)
  {
    UIGrowingLayoutBase*  pLayout(static_cast<UIGrowingLayoutBase*>(pUIElem));

    const char* pValue(0);

    // alignment
    int value(UIBuilder::GetXmlAlignment(pXml, "align"));
    if (value < UIBuilder::XA_NONE)
    {
      pLayout->SetAlignment(static_cast<UIElement::Alignment>(value - 1));
    }

    // grow direction
    pValue = pXml->Attribute("growDir");
    if (pValue != 0)
    {
      const uint32* pFind(std::find(karDirectionNameHash, karDirectionNameHash + 2, Hash::String(pValue)));

      int value(pFind - karDirectionNameHash);
      success = value < 2;
      if (success)
      {
        pLayout->SetGrowDir(static_cast<UIGrowingLayoutBase::GrowDir>(value));
      }
      else
      {
        XR_TRACE(UIBuilder,
          ("Invalid value in 'growDir' element for UIGrowingLayoutBase: %s",
            pValue));
      }
    }

    // spacing
    int spacing(0);
    if (success)
    {
      GetXmlIntAttribute(pXml, "spacing",
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
UIElement*  UIBCreateUIHorizontalLayout(AllocateCallback pAllocCb,
              void* pAllocCbData)
{
  return new (static_cast<UIHorizontalLayout*>((*pAllocCb)(sizeof(UIHorizontalLayout),
    pAllocCbData))) UIHorizontalLayout;
}

//==============================================================================
UIElement*  UIBCreateUIVerticalLayout(AllocateCallback pAllocCb,
              void* pAllocCbData)
{
  return new (static_cast<UIVerticalLayout*>((*pAllocCb)(sizeof(UIVerticalLayout),
    pAllocCbData))) UIVerticalLayout;
}

//==============================================================================
UIElement*  UIBCreateUIHorizontalScrollingLayout(AllocateCallback pAllocCb,
              void* pAllocCbData)
{
  return new (static_cast<UIHorizontalScrollingLayout*>((*pAllocCb)(sizeof(UIVerticalScrollingLayout),
    pAllocCbData))) UIHorizontalScrollingLayout;
}

//==============================================================================
bool  UIBInitUIHorizontalScrollingLayout(TiXmlElement* pXml, UIElement* pUIElem,
        UIContainer* pParent, const UIBuilder& builder)
{
  bool  success(UIBInitUIGrowingLayout(pXml, pUIElem, pParent, builder));
  if(success)
  {
    UIHorizontalScrollingLayout*  pLayout(static_cast<UIHorizontalScrollingLayout*>(pUIElem));

    int xOffset;
    if (GetXmlIntAttribute(pXml, "xOffset", pLayout->w, xOffset))
    {
      pLayout->SetXOffset(xOffset);
    }

    float sensitivity;
    if (GetXmlFloatAttribute(pXml, "sensitivity", sensitivity))
    {
      pLayout->SetSensitivity(sensitivity);
    }
  }

  return success;
}

//==============================================================================
UIElement*  UIBCreateUIVerticalScrollingLayout(AllocateCallback pAllocCb,
              void* pAllocCbData)
{
  return new (static_cast<UIVerticalScrollingLayout*>((*pAllocCb)(sizeof(UIVerticalScrollingLayout),
    pAllocCbData))) UIVerticalScrollingLayout;
}

//==============================================================================
bool  UIBInitUIVerticalScrollingLayout(TiXmlElement* pXml, UIElement* pUIElem,
        UIContainer* pParent, const UIBuilder& builder)
{
  bool  success(UIBInitUIGrowingLayout(pXml, pUIElem, pParent, builder));
  if(success)
  {
    UIVerticalScrollingLayout*  pLayout(static_cast<UIVerticalScrollingLayout*>(pUIElem));

    int yOffset;
    if (GetXmlIntAttribute(pXml, "yOffset", pLayout->h, yOffset))
    {
      pLayout->SetYOffset(yOffset);
    }

    float sensitivity;
    if (GetXmlFloatAttribute(pXml, "sensitivity", sensitivity))
    {
      pLayout->SetSensitivity(sensitivity);
    }
  }

  return success;
}

//==============================================================================
// grid layout
UIElement*  UIBCreateUIGridLayout(AllocateCallback pAllocCb,
              void* pAllocCbData)
{
  return new (static_cast<UIGridLayout*>((*pAllocCb)(sizeof(UIGridLayout),
    pAllocCbData))) UIGridLayout;
}

//==============================================================================
bool  UIBInitUIGridLayout(TiXmlElement* pXml, UIElement* pUIElem,
        UIContainer* pParent, const UIBuilder& builder)
{
  bool  success(UIBInitUIElement(pXml, pUIElem, pParent, builder));
  if(success)
  {
    UIGridLayout*  pLayout(static_cast<UIGridLayout*>(pUIElem));

    const char* pValue(0);
    int value;
  
    // rows
    if (success)
    {
      pValue = pXml->Attribute("rows", &value);
      success = pValue != 0 && CheckInt16(value) && value >= 0;

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
      pValue = pXml->Attribute("cols", &value);
      success = pValue != 0 && CheckInt16(value) && value >= 0;

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
    if (success && GetXmlIntAttribute(pXml, "wCell", pParent->w, value))
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
    if (success && GetXmlIntAttribute(pXml, "hCell", pParent->h, value))
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
    if (success && GetXmlIntAttribute(pXml, "colPadding",
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
    if (success && GetXmlIntAttribute(pXml, "rowPadding",
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
      value = UIBuilder::GetXmlAlignment(pXml, "hAlign");
      if (value < UIBuilder::XA_NONE)
      {
        pLayout->SetHorizontalAlignment(static_cast<UIElement::Alignment>(value - 1));
      }

      // vertical alignment
      value = UIBuilder::GetXmlAlignment(pXml, "vAlign");
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

const uint32  UIBuilder::kIncludeHash = Hash::String(kInclude);

const char* const UIBuilder::karpAlignValues[] =
{
  "low",
  "center",
  "high",
  "none"
};

const uint32 UIBuilder::karAlignValueHash[] =
{
  Hash::String(karpAlignValues[XA_LOW]),
  Hash::String(karpAlignValues[XA_CENTER]),
  Hash::String(karpAlignValues[XA_HIGH]),
  Hash::String(karpAlignValues[XA_NONE])
};

const char* const  UIBuilder::karpElementName[] =
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
  Sprite::Manager::Get,
  0,
  Font::Manager::Get,
  0,
  NewAllocate,
  0,
  NewDeallocate,
  0
};

//==============================================================================
int UIBuilder::GetXmlAlignment(TiXmlElement* pXml, const char* pAttribName)
{
  XR_ASSERT(UIBuilder, pXml != 0);
  XR_ASSERT(UIBuilder, pAttribName != 0);

  int value(XA_NONE);
  const char* pValue(pXml->Attribute(pAttribName));
  if (pValue != 0)
  {
    value = FindItemId(karAlignValueHash, kNumAlignValues, Hash::String(pValue));
    if (value >= kNumAlignValues) // if not found, default to none
    {
      XR_TRACE(UIBuilder, ("The value of '%s' is not a valid alignment.", pAttribName));
      value = XA_NONE;
    }
  }
  return value;
}

//==============================================================================
UIBuilder::UIBuilder(const Configuration& cfg)
: m_creators(),
  m_cfg(cfg),
  m_pRoot(0),
  m_depth(0),
  m_parLevels(0),
  m_handles()
{
  RegisterCreator(karpElementName[UI_SPACER], UIBCreateUISpacer,
    UIBInitUIElement, false);
  RegisterCreator(karpElementName[UI_BUTTON], UIBCreateUIButton,
    UIBInitUIButton, false);
  RegisterCreator(karpElementName[UI_CHECKBOX], UIBCreateUICheckBox,
    UIBInitUICheckBox, false);
  RegisterCreator(karpElementName[UI_RADIOBUTTON], UIBCreateUIRadioButton,
    UIBInitUIRadioButton, false);
  RegisterCreator(karpElementName[UI_LABEL], UIBCreateUILabel, UIBInitUILabel,
    false);
  RegisterCreator(karpElementName[UI_IMAGE], UIBCreateUIImage, UIBInitUIImage,
    false);
  RegisterCreator(karpElementName[UI_IMAGEPANEL], UIBCreateUIImagePanel,
    UIBInitUIImagePanel, false);
  RegisterCreator(karpElementName[UI_HPROGRESS], UIBCreateUIHorizontalProgressBar,
    UIBInitUIProgressBarBase, false);
  RegisterCreator(karpElementName[UI_VPROGRESS], UIBCreateUIVerticalProgressBar,
    UIBInitUIProgressBarBase, false);
  RegisterCreator(karpElementName[UI_HSLIDER], UIBCreateUIHorizontalSlider,
    UIBInitUISliderBase, false);
  RegisterCreator(karpElementName[UI_VSLIDER], UIBCreateUIVerticalSlider,
    UIBInitUISliderBase, false);

  // containers
  RegisterCreator(karpElementName[UI_ALIGNER], UIBCreateUIAligner,
    UIBInitUIAligner, true);
  RegisterCreator(karpElementName[UI_CASCADER], UIBCreateUICascader,
    UIBInitUICascader, true);
  RegisterCreator(karpElementName[UI_HLAYOUT], UIBCreateUIHorizontalLayout,
    UIBInitUIGrowingLayout, true);
  RegisterCreator(karpElementName[UI_VLAYOUT], UIBCreateUIVerticalLayout,
    UIBInitUIGrowingLayout, true);
  RegisterCreator(karpElementName[UI_HSCROLLINGLAYOUT],
    UIBCreateUIHorizontalScrollingLayout,
    UIBInitUIHorizontalScrollingLayout, true);
  RegisterCreator(karpElementName[UI_VSCROLLINGLAYOUT],
    UIBCreateUIVerticalScrollingLayout,
    UIBInitUIVerticalScrollingLayout, true);
  RegisterCreator(karpElementName[UI_GRIDLAYOUT], UIBCreateUIGridLayout,
    UIBInitUIGridLayout, true);
}

//==============================================================================
UIBuilder::~UIBuilder()
{
  Destroy();
  delete[]  m_parLevels;
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
  
  delete[]  m_parLevels;
  m_parLevels = new UIElementList[maxDepth];
  m_cfg.maxDepth = maxDepth;
}

//==============================================================================
void UIBuilder::SetGetSpriteCallback(Sprite::GetCallback pGetSpriteCb,
  void* pCbData)
{
  m_cfg.pGetSprite = pGetSpriteCb;
  m_cfg.pGetSpriteData = pCbData;
}

//==============================================================================
void UIBuilder::SetGetFontCallback(Font::GetCallback pGetFontCb,
  void* pCbData)
{
  m_cfg.pGetFont = pGetFontCb;
  m_cfg.pGetFontData = pCbData;
}

//==============================================================================
void  UIBuilder::SetAllocateCallback(AllocateCallback pAllocateCb, void* pCbData)
{
  m_cfg.pAllocate = pAllocateCb;
  m_cfg.pAllocateData  = pCbData;
}

//==============================================================================
void  UIBuilder::SetDeallocateCallback(DeallocateCallback pDeallocateCb, void* pCbData)
{
  m_cfg.pDeallocate = pDeallocateCb;
  m_cfg.pDeallocateData = pCbData;
}

//==============================================================================
void* UIBuilder::Allocate(int size)
{
  XR_ASSERTMSG(UIBuilder, m_cfg.pAllocate != 0,
    ("%s callback was not set.", "Allocate"));
  return (*m_cfg.pAllocate)(size, m_cfg.pAllocateData);
}

//==============================================================================
void  UIBuilder::Deallocate(void* pBuffer)
{
  XR_ASSERTMSG(UIBuilder, m_cfg.pDeallocate != 0,
    ("%s callback was not set.", "Deallocate"));
  (*m_cfg.pDeallocate)(pBuffer, m_cfg.pDeallocateData);
}

//==============================================================================
const Sprite* UIBuilder::GetSprite(const char* pName) const
{
  XR_ASSERTMSG(UIBuilder, m_cfg.pGetSprite != 0,
    ("%s callback was not set.", "GetSprite"));
  return (*m_cfg.pGetSprite)(pName, m_cfg.pGetSpriteData);
}

//==============================================================================
const Font* UIBuilder::GetFont(const char* pName) const
{
  XR_ASSERTMSG(UIBuilder, m_cfg.pGetFont != 0,
    ("%s callback was not set.", "GetFont"));
  return (*m_cfg.pGetFont)(pName, m_cfg.pGetFontData);
}

//==============================================================================
std::string UIBuilder::ProcessText(const char* pText) const
{
  if(m_cfg.pProcessText != 0)
  {
    return (*m_cfg.pProcessText)(pText, m_cfg.pProcessTextUser);
  }
  else
  {
    return pText;
  }
}

//==============================================================================
void  UIBuilder::RegisterCreator(const char* pName, CreateCallback pCreateCb,
        InitCallback pInitCb, bool isContainer)
{
  XR_ASSERT(UIBuilder, pName != 0);
  UICreatorRecord uicr =
  {
    pCreateCb,
    pInitCb,
    isContainer
#if defined IW_DEBUG
    , pName
#endif
  };
  m_creators[Hash::String(pName)] = uicr;
}

//==============================================================================
bool UIBuilder::RegisterNamedElement(const char* pName, UIElement* pUIElem)
{
  XR_ASSERT(UIBuilder, pName != 0);
  XR_ASSERT(UIBuilder, pUIElem != 0);

  uint32                hash(Hash::String(pName));
  ElementMap::iterator  iFind(m_handles.find(hash));
  bool                  success(iFind == m_handles.end());
  if (success)
  {
    ElementMap::value_type  vInsert(hash, pUIElem);
    m_handles.insert(vInsert);
  }
  else
  {
    success = iFind->first == hash;
  }
  XR_ASSERTMSG(UIBuilder, success, ("The handle '%s' is already taken", pName));
  
  return success;
}

//==============================================================================
bool  UIBuilder::Build(TiXmlElement* pXml, UIContainer& container)
{
  XR_ASSERT(IW_ASSERTION_CHANNEL_DEFAULT, pXml != 0);
  XR_ASSERT(IW_ASSERTION_CHANNEL_DEFAULT, m_cfg.pAllocate != 0);

  m_pRoot = &container;

  UIContainer base(container);
  base.w = Renderer::GetScreenWidth();
  base.h = Renderer::GetScreenHeight();

  RegisterNamedElement("root", m_pRoot);
  UIBInitUIElement(pXml, m_pRoot, &base, *this);

  int depth(0);
  return _Build(pXml, m_pRoot, depth);
}

//==============================================================================
bool  UIBuilder::_Build(TiXmlElement* pXml, UIContainer* pContainer, int& depth)
{
  XR_ASSERT(UIBuilder, pContainer != 0);

  pXml = pXml->FirstChildElement();
  bool  success(true);
  while (pXml != 0)
  {
    UIElement*  pUIElem(0);

    uint32  hash(Hash::String(pXml->Value()));
    CreatorMap::iterator  iFind(m_creators.find(hash));
    if (iFind != m_creators.end())
    {
      XR_TRACE(UIBuilder, ("Building a %s...", iFind->second.pName));

      const UICreatorRecord&  uicr(iFind->second);

      // create element (name)
      pUIElem = (*uicr.pCreateCb)(m_cfg.pAllocate, m_cfg.pAllocateData);
      success = pUIElem != 0;
      if (!success)
      {
        XR_TRACE(UIBuilder,
          ("Failed to create a %s, possibly due to insufficiend memory.",
            iFind->second.pName));
      }

      // setup element (attributes)
      if (success)
      {
        success = (*uicr.pInitCb)(pXml, pUIElem, pContainer, *this);
        if (!success)
        {
          XR_TRACE(UIBuilder,
            ("Failed to initialise a %s @ 0x%p. Destruction and deallocation has been attempted. Quitting build.",
              iFind->second.pName, pUIElem));

          pUIElem->~UIElement();
          Deallocate(pUIElem);
        }
      }

      if (success)
      {
        // postprocess
        _PostProcess(pXml, pUIElem);

        // add to parent
        pContainer->AddElement(pUIElem);

        m_parLevels[depth].push_back(pUIElem);

        // process container
        if (uicr.isContainer)
        {
          UIContainer*  pMyContainer(static_cast<UIContainer*>(pUIElem));

          ++depth;
          if(m_depth < depth)
          {
            m_depth = depth;
          }

          XR_ASSERTMSG(UIBuilder, depth < m_cfg.maxDepth,
            ("maxDepth (%d) too small, try a greater value.", m_cfg.maxDepth));
          success = _Build(pXml, pMyContainer, depth);
          _PostProcessContainer(pXml, pMyContainer);
          --depth;
        }
      }
    }
    else if (kIncludeHash == hash)
    {
      const char* pValue(pXml->Attribute("src"));
      success = pValue != 0;
      if(success)
      {
        TiXmlDocument doc;
        std::string fileName((m_cfg.pFormatFileName != 0) ?
          (*m_cfg.pFormatFileName)(pValue, m_cfg.pFormatFileNameUser) :
          pValue);
        
        success = doc.LoadFile(fileName.c_str()) &&
          doc.RootElement() != 0 &&
          _Build(doc.RootElement(), pContainer, depth);
      }
    }

    if (!success)
    {
      break;
    }

    pXml = pXml->NextSiblingElement();
  }

  return success;
}

//==============================================================================
void UIBuilder::Destroy()
{
  if (m_depth > 0)
  {
    XR_ASSERT(UIBuilder, m_parLevels != 0);
    
    if (m_pRoot != 0)
    {
      UIElementList&  lElems(m_parLevels[0]);
      for (UIElementList::iterator i0(lElems.begin()), i1(lElems.end());
        i0 != i1; ++i0)
      {
        m_pRoot->RemoveElement(*i0);
      }
    }
    
    for (int i = 0; i <= m_depth; ++i)
    {
      UIElementList&  lElems(m_parLevels[i]);
      for (UIElementList::iterator i0(lElems.begin()), i1(lElems.end());
        i0 != i1; ++i0)
      {
        UIElement*  p(*i0);
        p->~UIElement();

        (*m_cfg.pDeallocate)(p, m_cfg.pDeallocateData);
      }

      m_parLevels[i].clear();
    }

    m_depth = 0;
  }

  m_handles.clear();
}

//==============================================================================
UIElement* UIBuilder::GetElement(uint32 hash) const
{
  ElementMap::const_iterator iFind(m_handles.find(hash));
  return (iFind != m_handles.end()) ? iFind->second : 0;
}

//==============================================================================
UIElement* UIBuilder::GetElement(const char* pHandle) const
{
  XR_ASSERT(UIBuilder, pHandle != 0);
  return GetElement(Hash::String(pHandle));
}

//==============================================================================
void UIBuilder::_PostProcess(TiXmlElement* pXml, UIElement* pUIElem)
{
  const char* pValue(pXml->Attribute("handle"));
  if (pValue != 0)
  {
    RegisterNamedElement(pValue, pUIElem);
  }

  const UIElement*  pParent(pUIElem->GetParent());
  if(pParent != 0)
  {
    uint32  flags(GetXmlDimensionMask(pXml, "fillParent"));
    int   w((flags & XR_MASK_ID(XD_WIDTH)) != 0 ? pParent->w : pUIElem->w);
    int   h((flags & XR_MASK_ID(XD_HEIGHT)) != 0 ? pParent->h : pUIElem->h);
    pUIElem->SetSize(w, h);
  }
  
  pValue = pXml->Attribute("alignTo");
  if (pValue != 0)
  {
    UIElement*  pAlignTo(GetElement(pValue));
    XR_ASSERT(UIBuilder, pAlignTo != 0);

    int alignTo(XA_NONE);
    int xTarget(0), yTarget(0);

    // horizontal
    alignTo = GetXmlAlignment(pXml, "alignToHorizontal");
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

    int hAlignSide(GetXmlAlignment(pXml, "alignSideHorizontal"));
    if (hAlignSide == XA_NONE)
    {
      hAlignSide = alignTo;
    }

    // vertical
    alignTo = GetXmlAlignment(pXml, "alignToVertical");
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

    int vAlignSide(GetXmlAlignment(pXml, "alignSideVertical"));
    if (vAlignSide == XA_NONE)
    {
      vAlignSide = alignTo;
    }

    pUIElem->Align(xTarget, yTarget, static_cast<UIElement::Alignment>(hAlignSide - 1),
      static_cast<UIElement::Alignment>(vAlignSide - 1));
  }

}

//==============================================================================
void UIBuilder::_PostProcessContainer(TiXmlElement* pXml, UIContainer* pContainer)
{
  uint32 sizeToContentValue(GetXmlDimensionMask(pXml, "sizeToContent"));
  if (IsFullMask(sizeToContentValue, XR_MASK_ID(XD_WIDTH) | XR_MASK_ID(XD_HEIGHT)))
  {
    pContainer->SetSizeToContent();
  }
  else
  {
    if (IsFullMask(sizeToContentValue, XR_MASK_ID(XD_WIDTH)))
    {
      pContainer->SetWidthToContent();
    }

    if (IsFullMask(sizeToContentValue, XR_MASK_ID(XD_HEIGHT)))
    {   
      pContainer->SetHeightToContent();
    }
  }
}

//==============================================================================
void* NewAllocate(size_t size, void* pUser)
{
  return new Byte[size];
}

//==============================================================================
void  NewDeallocate(void* pMem, void* pUser)
{
  delete [] static_cast<Byte*>(pMem);
}

} // XR
