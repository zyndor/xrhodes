#ifndef XR_IUIRENDERER_HPP
#define XR_IUIRENDERER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "XR/Sprite.hpp"

namespace XR
{

class IUIRenderer
{
public:
  using Vertex = Vertex::Format<Vertex::Color0<Color>, SpriteVertexFormat>;

  virtual ~IUIRenderer()
  {}

  ///@brief Allocates and returns 4 vertices for a sprite of the given
  /// @a material to be written into.
  virtual Vertex* NewSprite(Material::Ptr const& material) =0;

  ///@brief Renders submitted and not yet rendered sprites.
  virtual void Render() =0;
};

}

#endif
