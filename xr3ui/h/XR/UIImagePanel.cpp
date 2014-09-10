#include <XR/maths.hpp>
#include "UIImagePanel.hpp"
#include "UIRenderer.hpp"

namespace XR
{

//==============================================================================
UIImagePanel::UIImagePanel()
: UIImage(),
  hSplit(.5f),
  vSplit(.5f)
{}

//==============================================================================
UIImagePanel::~UIImagePanel()
{}

//==============================================================================
void UIImagePanel::Render() const
{
  XR_ASSERTMSG(UIImagePanel, sprite.GetMaterial() != 0,
    ("Material needs to be set in UIImagePanel::sprite before Render()"));

  const RenderStream rsSpriteVerts(sprite.GetVertices());

  float hwSprite(sprite.GetHalfWidth());
  float wSprite(hwSprite * 2.0f);

  float hhSprite(sprite.GetHalfHeight());
  float hSprite(hhSprite * 2.0f);
  
  float xSplit(wSprite * hSplit);
  float ySplit(hSprite * vSplit);
  int16 ixSplit(static_cast<int16>(Round(xSplit)));
  int16 iySplit(static_cast<int16>(Round(ySplit)));
  
  int16 arXCoords[4] =
  {
    x + (int16)XR::Round(sprite.GetLeftPadding()),
    x + ixSplit,
    x + w - ixSplit,
    x + w - (int16)XR::Round(sprite.GetRightPadding())
  };

  int16 arYCoords[4] =
  {
    y + (int16)XR::Round(sprite.GetTopPadding()),
    y + iySplit,
    y + h - iySplit,
    y + h - (int16)XR::Round(sprite.GetBottomPadding())
  };
  
  const RenderStream& rsSpriteUVs(sprite.GetUVs());
  float arU[4];
  if (sprite.IsUVRotated())
  {
    arU[0] = rsSpriteUVs.GetX(Sprite::VI_SW);
    arU[3] = rsSpriteUVs.GetX(Sprite::VI_NW);
  }
  else
  {
    arU[0] = rsSpriteUVs.GetX(Sprite::VI_NW);
    arU[3] = rsSpriteUVs.GetX(Sprite::VI_NE);
  }

  arU[1] = Lerp(arU[0], arU[3], hSplit);
  arU[2] = arU[1];
  
  float arV[4];
  if (sprite.IsUVRotated())
  {
    arV[0] = rsSpriteUVs.GetY(Sprite::VI_SW);
    arV[3] = rsSpriteUVs.GetY(Sprite::VI_SE);
  }
  else
  {
    arV[0] = rsSpriteUVs.GetY(Sprite::VI_NW);
    arV[3] = rsSpriteUVs.GetY(Sprite::VI_SW);
  }

  arV[1] = Lerp(arV[0], arV[3], vSplit);
  arV[2] = arV[1];
  
  int ix0(0);
  int ix1(3);
  if (arXCoords[1] > arXCoords[2]) // ui width is smaller than sprite width
  {
    // scale sprite to ui
    float scale(w / wSprite);
    arXCoords[0] = x + Round(scale * (rsSpriteVerts.GetX(Sprite::VI_NW) + hwSprite));
    arXCoords[1] = x + Round(scale * (rsSpriteVerts.GetX(Sprite::VI_NE) + hwSprite));
    
    arU[1] = arU[3];
    ix1 = 1;
  }
  else
  {
    if (arXCoords[0] > arXCoords[1])
    {
      // ignore low half
      arU[2] = arU[0];
      ix0 += 2;
    }

    if (arXCoords[3] < arXCoords[2])
    {
      // ignore high half
      arU[1] = arU[3];
      ix1 -= 2;
    }
  }
  
  int iy0(0);
  int iy1(3);
  if (arYCoords[1] > arYCoords[2]) // ui width is smaller than sprite width
  {
    // scale sprite to ui
    float scale(h / hSprite);
    arYCoords[0] = y + Round(scale * (rsSpriteVerts.GetY(Sprite::VI_NW) + hhSprite));
    arYCoords[1] = y + Round(scale * (rsSpriteVerts.GetY(Sprite::VI_SW) + hhSprite));

    arV[1] = arV[3];
    iy1 = 1;
  }
  else
  {
    if (arYCoords[0] > arYCoords[1])
    {
      // ignore low half
      arV[2] = arV[0];
      iy0 += 2;
    }
    
    if (arYCoords[3] < arYCoords[2])
    {
      // ignore high half
      arV[1] = arV[3];
      iy1 -= 2;
    }
  }
  
  int hQuads(ix1 - ix0);
  int vQuads(iy1 - iy0);
  
  int numVertices(hQuads * vQuads * Sprite::kNumVertices);
  if (numVertices > 0)
  {
    RenderStream* pRsVerts(Renderer::AllocStream(RenderStream::F_VECTOR3, numVertices));
    RenderStream* pRsUVs(Renderer::AllocStream(RenderStream::F_VECTOR2, numVertices));

    int           numIndices(hQuads * vQuads * Sprite::kNumIndices);
    uint16*       parIndices(XR_RENDERER_ALLOC(uint16, numIndices));
    uint16*       pWriteIndex(parIndices);

    int           iWrite(0);
  
    int cell(0);
    for (int i = iy0; i < iy1; ++i)
    {
      for (int j = ix0; j < ix1; ++j)
      {
        pRsVerts->Set(iWrite + 0, Vector3(arXCoords[j], arYCoords[i], .0f));
        pRsVerts->Set(iWrite + 1, Vector3(arXCoords[j], arYCoords[i + 1], .0f));
        pRsVerts->Set(iWrite + 2, Vector3(arXCoords[j + 1], arYCoords[i + 1], .0f));
        pRsVerts->Set(iWrite + 3, Vector3(arXCoords[j + 1], arYCoords[i], .0f));
        
        pRsUVs->Set(iWrite + 0, Vector2(arU[j], arV[i]));
        pRsUVs->Set(iWrite + 1, Vector2(arU[j], arV[i + 1]));
        pRsUVs->Set(iWrite + 2, Vector2(arU[j + 1], arV[i + 1]));
        pRsUVs->Set(iWrite + 3, Vector2(arU[j + 1], arV[i]));
        
        iWrite += Sprite::kNumVertices;

        Sprite::CopyIndicesTo(pWriteIndex, cell * Sprite::kNumVertices); 
        pWriteIndex += Sprite::kNumIndices;
        ++cell;
      }
    }
    
    Renderer::SetMaterial(sprite.GetMaterial());
    Renderer::SetAmbientColor(color);
    
    Renderer::SetVertStream(*pRsVerts);
    Renderer::SetUVStream(*pRsUVs);
    
    // todo: calculate indices for all quads!
    Renderer::DrawPrims(PRIM_TRI_LIST, parIndices, numIndices);
  }
}

//==============================================================================
void UIImagePanel::Render( UIRenderer* pRenderer ) const
{
  XR_ASSERTMSG(UIImagePanel, sprite.GetMaterial() != 0,
    ("Material needs to be set in UIImagePanel::sprite before Render()"));

  const RenderStream& rsSpriteVerts(sprite.GetVertices());

  SVector2* pVerts(0);

  float hwSprite(sprite.GetHalfWidth());
  float wSprite(hwSprite * 2.0f);

  float hhSprite(sprite.GetHalfHeight());
  float hSprite(hhSprite * 2.0f);

  float xSplit(wSprite * hSplit);
  float ySplit(hSprite * vSplit);
  int16 ixSplit(static_cast<int16>(Round(xSplit)));
  int16 iySplit(static_cast<int16>(Round(ySplit)));
  
  int16 arXCoords[4] =
  {
    x + (int16)XR::Round(sprite.GetLeftPadding()),
    x + ixSplit,
    x + w - ixSplit,
    x + w - (int16)XR::Round(sprite.GetRightPadding())
  };

  int16 arYCoords[4] =
  {
    y + (int16)XR::Round(sprite.GetTopPadding()),
    y + iySplit,
    y + h - iySplit,
    y + h - (int16)XR::Round(sprite.GetBottomPadding())
  };

  const RenderStream& rsSpriteUVs(sprite.GetUVs());
  float arU[4];
  if (sprite.IsUVRotated())
  {
    arU[0] = rsSpriteUVs.GetX(Sprite::VI_SW);
    arU[3] = rsSpriteUVs.GetX(Sprite::VI_NW);
  }
  else
  {
    arU[0] = rsSpriteUVs.GetX(Sprite::VI_NW);
    arU[3] = rsSpriteUVs.GetX(Sprite::VI_NE);
  }

  arU[1] = Lerp(arU[0], arU[3], hSplit);
  arU[2] = arU[1];

  float arV[4];
  if (sprite.IsUVRotated())
  {
    arV[0] = rsSpriteUVs.GetY(Sprite::VI_SW);
    arV[3] = rsSpriteUVs.GetY(Sprite::VI_SE);
  }
  else
  {
    arV[0] = rsSpriteUVs.GetY(Sprite::VI_NW);
    arV[3] = rsSpriteUVs.GetY(Sprite::VI_SW);
  }

  arV[1] = Lerp(arV[0], arV[3], vSplit);
  arV[2] = arV[1];

  int ix0(0);
  int ix1(3);
  if (arXCoords[1] > arXCoords[2]) // ui width is smaller than sprite width
  {
    // scale sprite to ui
    float scale(w / wSprite);
    arXCoords[0] = x + Round(scale * (rsSpriteVerts.GetX(Sprite::VI_NW) + hwSprite));
    arXCoords[1] = x + Round(scale * (rsSpriteVerts.GetX(Sprite::VI_NE) + hwSprite));

    arU[1] = arU[3];
    ix1 = 1;
  }
  else
  {
    if (arXCoords[0] > arXCoords[1])
    {
      // ignore low half
      arU[2] = arU[0];
      ix0 += 2;
    }

    if (arXCoords[3] < arXCoords[2])
    {
      // ignore high half
      arU[1] = arU[3];
      ix1 -= 2;
    }
  }

  int iy0(0);
  int iy1(3);
  if (arYCoords[1] > arYCoords[2]) // ui width is smaller than sprite width
  {
    // scale sprite to ui
    float scale(h / hSprite);
    arYCoords[0] = y + Round(scale * (rsSpriteVerts.GetY(Sprite::VI_NW) + hhSprite));
    arYCoords[1] = y + Round(scale * (rsSpriteVerts.GetY(Sprite::VI_SW) + hhSprite));

    arV[1] = arV[3];
    iy1 = 1;
  }
  else
  {
    if (arYCoords[0] > arYCoords[1])
    {
      // ignore low half
      arV[2] = arV[0];
      iy0 += 2;
    }

    if (arYCoords[3] < arYCoords[2])
    {
      // ignore high half
      arV[1] = arV[3];
      iy1 -= 2;
    }
  }

  int hQuads(ix1 - ix0);
  int vQuads(iy1 - iy0);

  int numVertices(hQuads * vQuads * Sprite::kNumVertices);
  if (numVertices > 0)
  {
    RenderStream  rsUVs;
    for (int i = iy0; i < iy1; ++i)
    {
      for (int j = ix0; j < ix1; ++j)
      {
        RenderStream  rsVerts(pRenderer->NewSprite(sprite.GetMaterial(), color, rsUVs));

        rsVerts.Set(0, Vector3(arXCoords[j], arYCoords[i], .0f));
        rsVerts.Set(1, Vector3(arXCoords[j], arYCoords[i + 1], .0f));
        rsVerts.Set(2, Vector3(arXCoords[j + 1], arYCoords[i + 1], .0f));
        rsVerts.Set(3, Vector3(arXCoords[j + 1], arYCoords[i], .0f));

        rsUVs.Set(0, Vector2(arU[j], arV[i]));
        rsUVs.Set(1, Vector2(arU[j], arV[i + 1]));
        rsUVs.Set(2, Vector2(arU[j + 1], arV[i + 1]));
        rsUVs.Set(3, Vector2(arU[j + 1], arV[i]));
      }
    }
  }
}

} // XR
