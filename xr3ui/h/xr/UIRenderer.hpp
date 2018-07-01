#ifndef XR_UIRENDERER_HPP
#define XR_UIRENDERER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include "IUIRenderer.hpp"

namespace xr
{

//==============================================================================
///@brief Implementation of IUIRenderer which arranges the UIElements submitted
/// for rendering in batches according to their material, in order to try to
/// minimize state changes.
class UIRenderer: public IUIRenderer
{
  XR_NONCOPY_DECL(UIRenderer)

public:
  // structors
  UIRenderer();
  ~UIRenderer();

  // general use
  ///@brief Creates storage for @a numSprites sprites.
  void  Init(int numSprites);

  ///@return  The capacity of the renderer; the number of sprites you can batch.
  int GetNumSprites() const;

  ///@return  The number of sprites already consumed.
  int GetNumSpritesConsumed() const;

  ///@return  The number of sprites already rendered.
  int GetNumSpritesRendered() const;

  ///@brief   Records the material for a new sprite and returns a buffer
  /// for the client to populate with Sprite vertex information.
  ///@return  A FloatBuffer that you can write the 4 vertices to.
  ///@note    An assertion is tripped when NewSprite() is called more than the
  /// renderer's capacity, without Clear().
  Vertex*  NewSprite(Material::Ptr const& material);

  ///@brief   Renders all unrendered sprites and marks them as rendered.
  ///@note    To render every sprite repeatedly, ResetRendering() when
  /// GetNumSpritesRendered() == GetNumSprites(), and only THEN Render().
  void  Render();

  ///@brief   Resets all sprites to be unrendered so that a subsequent Render()
  /// call will render everything again.
  void  ResetRendering();

  ///@brief   Resets all sprite counters.
  void  Clear();

  ///@brief   Destroys the internal storage of sprite data.
  void  Shutdown();

protected:
  // data
  int m_numSprites;
  std::vector<Material::Ptr> m_materials;
  FloatBuffer m_vertices;
  Gfx::IndexBufferHandle m_ibo;

  int m_numSpritesRenderable;
  int m_numSpritesRendered;
};

//==============================================================================
// implementation
//==============================================================================
inline
int UIRenderer::GetNumSprites() const
{
  return m_numSprites;
}

//==============================================================================
inline
int UIRenderer::GetNumSpritesConsumed() const
{
  return m_numSpritesRenderable;
}

//==============================================================================
inline
int UIRenderer::GetNumSpritesRendered() const
{
  return m_numSpritesRendered;
}

} // xr

#endif // XR_UIRENDERER_HPP
