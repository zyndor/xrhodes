//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_UIRENDERER_HPP
#define XR_UIRENDERER_HPP

#include "XR/Sprite.hpp"
#include "XR/IndexMesh.hpp"

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
  ///@return  A FloatBuffer that you can write the 4 vertices to.
  ///@note    An assertion is tripped when any combination of NewSprite()
  /// methods are called more than the renderer's capacity without Clear().
  FloatBuffer  NewSprite(Material* pMaterial, const FloatBuffer& fbUV, Color color);
    
  ///@brief   Records the material and the color as passed. @a fbUV will be
  /// set to a buffer of 4 UVs, which you can subsequently fill out.
  /// The internal sprite counter is then incremented.
  ///@return  A FloatBuffer that you can write the 4 vertices to.
  ///@note    An assertion is tripped when any combination of NewSprite()
  /// methods are called more than the renderer's capacity without Clear().
  FloatBuffer  NewSprite(Material* pMaterial, Color color, FloatBuffer& fbUV);

  ///@brief   Records the material, @a fbUVs and @a fbColor sill be set to
  /// buffers of 4 UVs and Colors, which you can subsequently fill out.
  /// The internal sprite counter is then incremented.
  ///@return  A FloatBuffer that you can write the 4 vertices to.
  ///@note    An assertion is tripped when any combination of NewSprite()
  /// methods are called more than the renderer's capacity without Clear().
  FloatBuffer  NewSprite(Material* pMaterial, FloatBuffer& fbUV,
    FloatBuffer& fbColor);

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
  FloatBuffer   m_colors;
  
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
