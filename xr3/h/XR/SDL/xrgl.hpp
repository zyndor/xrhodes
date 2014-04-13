//
// Nuclear Heart Games
// XRhodes
//
// xrgl.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    14/01/2014
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_XRGL_HPP
#define XR_XRGL_HPP

#include  <GL/glew.h>
#include  <SDL2/SDL_video.h>
#include  <SDL2/SDL_opengl.h>
#include  "types.hpp"
#include  "Log.hpp"

namespace XR {

//==============================================================================
inline
  void  CheckGLError(const char* pText)
{
  GLint error(glGetError());
  if(error != 0)
  {
    XR_TRACE(CheckGLError, ("%s: %d", pText, error));
  }
  XR_ASSERTMSG(GL, error == GL_NONE, ("OpenGL Error: %d (attempting: %s)",
    error, pText));
}

} // XR

//==============================================================================
#define XR_GL_CALL(theCall)  (theCall); XR::CheckGLError( #theCall )

#endif // XR_XRGL_HPP
