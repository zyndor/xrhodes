//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_XRGL_HPP
#define XR_XRGL_HPP

#include <XR/Log.hpp>
#include <XR/debug.hpp>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <SDL_video.h>

namespace XR
{

//==============================================================================
inline
GLint  CheckGLError(const char* pText)
{
  GLint error(glGetError());
  if (error != 0)
  {
    XR_TRACE(CheckGLError, ("%s: %d", pText, error));
  }
  XR_ASSERTMSG(GL, error == GL_NONE, ("OpenGL Error: %d (attempting: %s)",
    error, pText));
  return error;
}

//==============================================================================
#define XR_GL_CALL(theCall)  theCall; CheckGLError( #theCall )

//==============================================================================
#define XR_GL_CALL_RESULT(theCall, result)  theCall; result = CheckGLError( #theCall )

}

#endif // XR_XRGL_HPP
