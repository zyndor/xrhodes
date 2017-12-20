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
void UIImagePanel::Render(IUIRenderer& renderer) const
{
  XR_ASSERTMSG(UIImagePanel, sprite.GetMaterial() != nullptr,
    ("Material needs to be set in UIImagePanel::sprite before Render()"));

  auto spriteVerts = sprite.GetVertices();

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

  float arU[4];
  if (sprite.IsUVRotated())
  {
    arU[0] = spriteVerts[Sprite::VI_SW].uv0.x;
    arU[3] = spriteVerts[Sprite::VI_NW].uv0.x;
  }
  else
  {
    arU[0] = spriteVerts[Sprite::VI_NW].uv0.x;
    arU[3] = spriteVerts[Sprite::VI_NE].uv0.x;
  }

  arU[1] = Lerp(arU[0], arU[3], hSplit);
  arU[2] = arU[1];

  float arV[4];
  if (sprite.IsUVRotated())
  {
    arV[0] = spriteVerts[Sprite::VI_SW].uv0.y;
    arV[3] = spriteVerts[Sprite::VI_SE].uv0.y;
  }
  else
  {
    arV[0] = spriteVerts[Sprite::VI_NW].uv0.y;
    arV[3] = spriteVerts[Sprite::VI_SW].uv0.y;
  }

  arV[1] = Lerp(arV[0], arV[3], vSplit);
  arV[2] = arV[1];

  int ix0(0);
  int ix1(3);
  if (arXCoords[1] > arXCoords[2]) // ui width is smaller than sprite width
  {
    // scale sprite to ui
    float scale(w / wSprite);
    arXCoords[0] = x + Round(scale * (spriteVerts[Sprite::VI_NW].pos.x + hwSprite));
    arXCoords[1] = x + Round(scale * (spriteVerts[Sprite::VI_NE].pos.x + hwSprite));

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
    arYCoords[0] = y + Round(scale * (spriteVerts[Sprite::VI_NW].pos.y + hhSprite));
    arYCoords[1] = y + Round(scale * (spriteVerts[Sprite::VI_SW].pos.y + hhSprite));

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
        auto verts = renderer.NewSprite(sprite.GetMaterial());

        verts[0].pos = Vector3(arXCoords[j], arYCoords[i], .0f);
        verts[1].pos = Vector3(arXCoords[j], arYCoords[i + 1], .0f);
        verts[2].pos = Vector3(arXCoords[j + 1], arYCoords[i + 1], .0f);
        verts[3].pos = Vector3(arXCoords[j + 1], arYCoords[i], .0f);

        verts[0].uv0 = Vector2(arU[j], arV[i]);
        verts[1].uv0 = Vector2(arU[j], arV[i + 1]);
        verts[2].uv0 = Vector2(arU[j + 1], arV[i + 1]);
        verts[3].uv0 = Vector2(arU[j + 1], arV[i]);

        verts[0].color0 = color;
        verts[1].color0 = color;
        verts[2].color0 = color;
        verts[3].color0 = color;
      }
    }
  }
}

} // XR
