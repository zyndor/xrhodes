//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "xr/maths.hpp"
#include "xr/UIImagePanel.hpp"
#include "xr/UIRenderer.hpp"

namespace xr
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
  XR_ASSERTMSG(UIImagePanel, material != nullptr,
    ("Material needs to be set in UIImagePanel::sprite before Render()"));

  auto spriteVerts = sprite.GetVertices();

  float hwSprite = sprite.GetHalfWidth();
  float wSprite = hwSprite * 2.0f;

  float hhSprite = sprite.GetHalfHeight();
  float hSprite = hhSprite * 2.0f;

  float xSplit = wSprite * hSplit;
  float ySplit = hSprite * vSplit;
  int32_t ixSplit = static_cast<int32_t>(std::round(xSplit));
  int32_t iySplit = static_cast<int32_t>(std::round(ySplit));

  float xCoords[4] =
  {
    float(x + std::round(sprite.GetLeftPadding())),
    float(x + ixSplit),
    float(x + w - ixSplit),
    float(x + w - std::round(sprite.GetRightPadding()))
  };

  float yCoords[4] =
  {
    float(y + std::round(sprite.GetTopPadding())),
    float(y + iySplit),
    float(y + h - iySplit),
    float(y + h - std::round(sprite.GetBottomPadding()))
  };

  float uCoords[4];
  if (sprite.IsUVRotated())
  {
    uCoords[0] = spriteVerts[Quad::Vertex::SW].uv0.x;
    uCoords[3] = spriteVerts[Quad::Vertex::NW].uv0.x;
  }
  else
  {
    uCoords[0] = spriteVerts[Quad::Vertex::NW].uv0.x;
    uCoords[3] = spriteVerts[Quad::Vertex::NE].uv0.x;
  }

  uCoords[1] = Lerp(uCoords[0], uCoords[3], hSplit);
  uCoords[2] = uCoords[1];

  float vCoords[4];
  if (sprite.IsUVRotated())
  {
    vCoords[0] = spriteVerts[Quad::Vertex::SW].uv0.y;
    vCoords[3] = spriteVerts[Quad::Vertex::SE].uv0.y;
  }
  else
  {
    vCoords[0] = spriteVerts[Quad::Vertex::NW].uv0.y;
    vCoords[3] = spriteVerts[Quad::Vertex::SW].uv0.y;
  }

  vCoords[1] = Lerp(vCoords[0], vCoords[3], vSplit);
  vCoords[2] = vCoords[1];

  int32_t ix0 = 0;
  int32_t ix1 = 3;
  if (xCoords[1] > xCoords[2]) // ui width is smaller than sprite width
  {
    // scale sprite to ui
    float scale = w / wSprite;
    xCoords[0] = x + std::round(scale * (spriteVerts[Quad::Vertex::NW].pos.x + hwSprite));
    xCoords[1] = x + std::round(scale * (spriteVerts[Quad::Vertex::NE].pos.x + hwSprite));

    uCoords[1] = uCoords[3];
    ix1 = 1;
  }
  else
  {
    if (xCoords[0] > xCoords[1])
    {
      // ignore low half
      uCoords[2] = uCoords[0];
      ix0 += 2;
    }

    if (xCoords[3] < xCoords[2])
    {
      // ignore high half
      uCoords[1] = uCoords[3];
      ix1 -= 2;
    }
  }

  int32_t iy0 = 0;
  int32_t iy1 = 3;
  if (yCoords[1] > yCoords[2]) // ui width is smaller than sprite width
  {
    // scale sprite to ui
    float scale = h / hSprite;
    yCoords[0] = y + std::round(scale * (spriteVerts[Quad::Vertex::NW].pos.y + hhSprite));
    yCoords[1] = y + std::round(scale * (spriteVerts[Quad::Vertex::SW].pos.y + hhSprite));

    vCoords[1] = vCoords[3];
    iy1 = 1;
  }
  else
  {
    if (yCoords[0] > yCoords[1])
    {
      // ignore low half
      vCoords[2] = vCoords[0];
      iy0 += 2;
    }

    if (yCoords[3] < yCoords[2])
    {
      // ignore high half
      vCoords[1] = vCoords[3];
      iy1 -= 2;
    }
  }

  int32_t hQuads = ix1 - ix0;
  int32_t vQuads = iy1 - iy0;

  const int numVertices = hQuads * vQuads * Quad::Vertex::kCount;
  if (numVertices > 0)
  {
    FloatBuffer  fbUVs;
    for (int i = iy0; i < iy1; ++i)
    {
      for (int j = ix0; j < ix1; ++j)
      {
        auto verts = renderer.NewSprite(material);

        verts[0].pos = Vector3(xCoords[j], yCoords[i], .0f);
        verts[1].pos = Vector3(xCoords[j], yCoords[i + 1], .0f);
        verts[2].pos = Vector3(xCoords[j + 1], yCoords[i + 1], .0f);
        verts[3].pos = Vector3(xCoords[j + 1], yCoords[i], .0f);

        verts[0].uv0 = Vector2(uCoords[j], vCoords[i]);
        verts[1].uv0 = Vector2(uCoords[j], vCoords[i + 1]);
        verts[2].uv0 = Vector2(uCoords[j + 1], vCoords[i + 1]);
        verts[3].uv0 = Vector2(uCoords[j + 1], vCoords[i]);

        verts[0].color0 = color;
        verts[1].color0 = color;
        verts[2].color0 = color;
        verts[3].color0 = color;
      }
    }
  }
}

} // xr
