//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_UIRENDERER_HPP
#define XR_UIRENDERER_HPP

#include <XR/Sprite.hpp>
#include <XR/IndexMesh.hpp>

namespace XR
{

//==============================================================================
class UIRenderer: protected IndexMesh
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
  
  ///@brief   Records the material and copies the uvs and color as passed.
  /// The internal sprite counter is then incremented.
  ///@return  A pointer to the RenderStream that you can write the 4 vertices to.
  ///@note    An assertion is tripped when any combination of NewSprite()
  /// methods are called more than the renderer's capacity without Clear().
  RenderStream  NewSprite(Material* pMaterial, const RenderStream& rsUV, Color color);
    
  ///@brief   Records the material and the color as passed. Writes a
  /// pointer to the 4 uvs to @a parUV, which you can subsequently fill out.
  /// The internal sprite counter is then incremented.
  ///@return  A pointer to the RenderStream that you can write the 4 vertices to.
  ///@note    An assertion is tripped when any combination of NewSprite()
  /// methods are called more than the renderer's capacity without Clear().
  RenderStream  NewSprite(Material* pMaterial, Color color, RenderStream& rsUV);

  ///@brief   Records the material, then writes a pointer to the 4 uvs to
  /// @a parUV and 4 colours to @a parCol, which you can subsequently fill out.
  /// The internal sprite counter is then incremented.
  ///@return  A pointer to the RenderStream vectors that you can write the 4 vertices to.
  ///@note    An assertion is tripped when any combination of NewSprite()
  /// methods are called more than the renderer's capacity without Clear().
  RenderStream  NewSprite(Material* pMaterial, RenderStream& rsUV,
    RenderStream& rsColor);

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
  int           m_numSprites;
  Material**    m_parpMaterial;
  RenderStream  m_colors;
  
  int           m_numSpritesConsumed;
  int           m_numSpritesRendered;
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
  return m_numSpritesConsumed;
}

//==============================================================================
inline
int UIRenderer::GetNumSpritesRendered() const
{
  return m_numSpritesRendered;
}

} // XR

#endif // XR_UIRENDERER_HPP
