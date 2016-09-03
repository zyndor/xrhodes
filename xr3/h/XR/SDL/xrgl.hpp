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

#include <XR/Log.hpp>
#include <XR/types.hpp>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <SDL_video.h>

namespace XR {

//==============================================================================
inline
void  CheckGLError(const char* pText)
{
  GLint error(glGetError());
  if (error != 0)
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
