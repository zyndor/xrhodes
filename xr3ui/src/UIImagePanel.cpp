//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/maths.hpp"
#include "XR/UIImagePanel.hpp"
#include "XR/UIRenderer.hpp"

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
  XR_ASSERTMSG(UIImagePanel, sprite.GetMaterial() != nullptr,
    ("Material needs to be set in UIImagePanel::sprite before Render()"));

  const FloatBuffer fbSpriteVerts(sprite.GetVertices());

  float hwSprite(sprite.GetHalfWidth());
  float wSprite(hwSprite * 2.0f);

  float hhSprite(sprite.GetHalfHeight());
  float hSprite(hhSprite * 2.0f);
  
  float xSplit(wSprite * hSplit);
  float ySplit(hSprite * vSplit);
  int ixSplit(static_cast<int>(Round(xSplit)));
  int iySplit(static_cast<int>(Round(ySplit)));
  
  int arXCoords[4] =
  {
    x + (int)Round(sprite.GetLeftPadding()),
    x + ixSplit,
    x + w - ixSplit,
    x + w - (int)Round(sprite.GetRightPadding())
  };

  int arYCoords[4] =
  {
    y + (int)Round(sprite.GetTopPadding()),
    y + iySplit,
    y + h - iySplit,
    y + h - (int)Round(sprite.GetBottomPadding())
  };
  
  const FloatBuffer& fbSpriteUVs(sprite.GetUVs());
  float arU[4];
  if (sprite.IsUVRotated())
  {
    arU[0] = fbSpriteUVs.Get<Vector2>(Sprite::VI_SW).x;
    arU[3] = fbSpriteUVs.Get<Vector2>(Sprite::VI_NW).x;
  }
  else
  {
    arU[0] = fbSpriteUVs.Get<Vector2>(Sprite::VI_NW).x;
    arU[3] = fbSpriteUVs.Get<Vector2>(Sprite::VI_NE).x;
  }

  arU[1] = Lerp(arU[0], arU[3], hSplit);
  arU[2] = arU[1];
  
  float arV[4];
  if (sprite.IsUVRotated())
  {
    arV[0] = fbSpriteUVs.Get<Vector2>(Sprite::VI_SW).y;
    arV[3] = fbSpriteUVs.Get<Vector2>(Sprite::VI_SE).y;
  }
  else
  {
    arV[0] = fbSpriteUVs.Get<Vector2>(Sprite::VI_NW).y;
    arV[3] = fbSpriteUVs.Get<Vector2>(Sprite::VI_SW).y;
  }

  arV[1] = Lerp(arV[0], arV[3], vSplit);
  arV[2] = arV[1];
  
  int ix0(0);
  int ix1(3);
  if (arXCoords[1] > arXCoords[2]) // ui width is smaller than sprite width
  {
    // scale sprite to ui
    float scale(w / wSprite);
    arXCoords[0] = x + Round(scale * (fbSpriteVerts.Get<Vector3>(Sprite::VI_NW).x + hwSprite));
    arXCoords[1] = x + Round(scale * (fbSpriteVerts.Get<Vector3>(Sprite::VI_NE).x + hwSprite));
    
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
    arYCoords[0] = y + Round(scale * (fbSpriteVerts.Get<Vector3>(Sprite::VI_NW).y + hhSprite));
    arYCoords[1] = y + Round(scale * (fbSpriteVerts.Get<Vector3>(Sprite::VI_SW).y + hhSprite));

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
    FloatBuffer* pFbVerts(Renderer::AllocBuffer(sizeof(Vector3), numVertices));
    FloatBuffer* pFbUVs(Renderer::AllocBuffer(sizeof(Vector2), numVertices));

    int           numIndices(hQuads * vQuads * Sprite::kNumIndices);
    uint16_t*     parIndices(XR_RENDERER_ALLOC(uint16_t, numIndices));
    uint16_t*     pWriteIndex(parIndices);

    int           iWrite(0);
  
    int cell(0);
    for (int i = iy0; i < iy1; ++i)
    {
      for (int j = ix0; j < ix1; ++j)
      {
        pFbVerts->Set(iWrite + 0, Vector3(arXCoords[j], arYCoords[i], .0f));
        pFbVerts->Set(iWrite + 1, Vector3(arXCoords[j], arYCoords[i + 1], .0f));
        pFbVerts->Set(iWrite + 2, Vector3(arXCoords[j + 1], arYCoords[i + 1], .0f));
        pFbVerts->Set(iWrite + 3, Vector3(arXCoords[j + 1], arYCoords[i], .0f));
        
        pFbUVs->Set(iWrite + 0, Vector2(arU[j], arV[i]));
        pFbUVs->Set(iWrite + 1, Vector2(arU[j], arV[i + 1]));
        pFbUVs->Set(iWrite + 2, Vector2(arU[j + 1], arV[i + 1]));
        pFbUVs->Set(iWrite + 3, Vector2(arU[j + 1], arV[i]));
        
        iWrite += Sprite::kNumVertices;

        Sprite::CopyIndicesTo(pWriteIndex, cell * Sprite::kNumVertices); 
        pWriteIndex += Sprite::kNumIndices;
        ++cell;
      }
    }
    
    sprite.GetMaterial()->Apply();
    Renderer::SetAmbientColor(color);
    
    Renderer::SetVertStream(*pFbVerts);
    Renderer::SetUVStream(*pFbUVs);
    
    // todo: calculate indices for all quads!
    Renderer::DrawPrims(PrimType::TRI_LIST, parIndices, numIndices);
  }
}

//==============================================================================
void UIImagePanel::Render( UIRenderer* pRenderer ) const
{
  XR_ASSERTMSG(UIImagePanel, sprite.GetMaterial() != nullptr,
    ("Material needs to be set in UIImagePanel::sprite before Render()"));

  const FloatBuffer& fbSpriteVerts(sprite.GetVertices());

  float hwSprite(sprite.GetHalfWidth());
  float wSprite(hwSprite * 2.0f);

  float hhSprite(sprite.GetHalfHeight());
  float hSprite(hhSprite * 2.0f);

  float xSplit(wSprite * hSplit);
  float ySplit(hSprite * vSplit);
  int ixSplit(static_cast<int>(Round(xSplit)));
  int iySplit(static_cast<int>(Round(ySplit)));
  
  int arXCoords[4] =
  {
    x + (int)Round(sprite.GetLeftPadding()),
    x + ixSplit,
    x + w - ixSplit,
    x + w - (int)Round(sprite.GetRightPadding())
  };

  int arYCoords[4] =
  {
    y + (int)Round(sprite.GetTopPadding()),
    y + iySplit,
    y + h - iySplit,
    y + h - (int)Round(sprite.GetBottomPadding())
  };

  const FloatBuffer& fbSpriteUVs(sprite.GetUVs());
  float arU[4];
  if (sprite.IsUVRotated())
  {
    arU[0] = fbSpriteUVs.Get<Vector2>(Sprite::VI_SW).x;
    arU[3] = fbSpriteUVs.Get<Vector2>(Sprite::VI_NW).x;
  }
  else
  {
    arU[0] = fbSpriteUVs.Get<Vector2>(Sprite::VI_NW).x;
    arU[3] = fbSpriteUVs.Get<Vector2>(Sprite::VI_NE).x;
  }

  arU[1] = Lerp(arU[0], arU[3], hSplit);
  arU[2] = arU[1];

  float arV[4];
  if (sprite.IsUVRotated())
  {
    arV[0] = fbSpriteUVs.Get<Vector2>(Sprite::VI_SW).y;
    arV[3] = fbSpriteUVs.Get<Vector2>(Sprite::VI_SE).y;
  }
  else
  {
    arV[0] = fbSpriteUVs.Get<Vector2>(Sprite::VI_NW).y;
    arV[3] = fbSpriteUVs.Get<Vector2>(Sprite::VI_SW).y;
  }

  arV[1] = Lerp(arV[0], arV[3], vSplit);
  arV[2] = arV[1];

  int ix0(0);
  int ix1(3);
  if (arXCoords[1] > arXCoords[2]) // ui width is smaller than sprite width
  {
    // scale sprite to ui
    float scale(w / wSprite);
    arXCoords[0] = x + Round(scale * (fbSpriteVerts.Get<Vector3>(Sprite::VI_NW).x + hwSprite));
    arXCoords[1] = x + Round(scale * (fbSpriteVerts.Get<Vector3>(Sprite::VI_NE).x + hwSprite));

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
    arYCoords[0] = y + Round(scale * (fbSpriteVerts.Get<Vector3>(Sprite::VI_NW).y + hhSprite));
    arYCoords[1] = y + Round(scale * (fbSpriteVerts.Get<Vector3>(Sprite::VI_SW).y + hhSprite));

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
    FloatBuffer  fbUVs;
    for (int i = iy0; i < iy1; ++i)
    {
      for (int j = ix0; j < ix1; ++j)
      {
        FloatBuffer  fbVerts(pRenderer->NewSprite(sprite.GetMaterial(), color, fbUVs));

        fbVerts.Set(0, Vector3(arXCoords[j], arYCoords[i], .0f));
        fbVerts.Set(1, Vector3(arXCoords[j], arYCoords[i + 1], .0f));
        fbVerts.Set(2, Vector3(arXCoords[j + 1], arYCoords[i + 1], .0f));
        fbVerts.Set(3, Vector3(arXCoords[j + 1], arYCoords[i], .0f));

        fbUVs.Set(0, Vector2(arU[j], arV[i]));
        fbUVs.Set(1, Vector2(arU[j], arV[i + 1]));
        fbUVs.Set(2, Vector2(arU[j + 1], arV[i + 1]));
        fbUVs.Set(3, Vector2(arU[j + 1], arV[i]));
      }
    }
  }
}

} // XR
