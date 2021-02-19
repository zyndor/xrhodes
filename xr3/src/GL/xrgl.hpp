#ifndef XR_XRGL_HPP
#define XR_XRGL_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/debug.hpp"
#include "gl_core_4_5.h"
#include "SDL2/SDL_opengl.h"

namespace xr
{
namespace gl
{

GLint CheckError(char const* text);

void SwitchEnabledState(GLenum name, bool state);

} // gl
} // xr

//==============================================================================
#define XR_GL_CALL(theCall)  theCall; xr::gl::CheckError( #theCall )

//==============================================================================
#define XR_GL_CALL_RESULT(theCall, result)\
  theCall;\
  result = xr::gl::CheckError( #theCall )

//==============================================================================
#define XR_GL_IGNORE_ERROR (void)glGetError();

//==============================================================================
// inline
//==============================================================================
namespace xr
{
namespace gl
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

} // gl
} // xr

#endif // XR_XRGL_HPP
