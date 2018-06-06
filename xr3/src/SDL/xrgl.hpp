#ifndef XR_XRGL_HPP
#define XR_XRGL_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include "xr/debug.hpp"
#include "gl_core_4_5.h"
#include "SDL_opengl.h"

namespace xr
{
namespace GL
{

GLint CheckError(char const* text);

void SwitchEnabledState(GLenum name, bool state);

} // GL
} // XR

//==============================================================================
#define XR_GL_CALL(theCall)  theCall; xr::GL::CheckError( #theCall )

//==============================================================================
#define XR_GL_CALL_RESULT(theCall, result)\
  theCall;\
  result = xr::GL::CheckError( #theCall )

//==============================================================================
#define XR_GL_IGNORE_ERROR (void)glGetError();

//==============================================================================
// inline
//==============================================================================
namespace xr
{
namespace GL
{

//==============================================================================
inline
GLint  CheckError(char const* text)
{
  GLint error = glGetError();
  if (error != GL_NO_ERROR)
  {
    XR_ERROR(("OpenGL error 0x%x occurred attempting '%s'.", error, text));
  }
  return error;
}

//==============================================================================
inline
void SwitchEnabledState(GLenum name, bool state)
{
  auto fn = state ? glEnable : glDisable;
  XR_GL_CALL(fn(name));
}

} // GL
} // XR


#endif // XR_XRGL_HPP
