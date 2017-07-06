//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_XRGL_HPP
#define XR_XRGL_HPP

#include <XR/debug.hpp>
#include <GL/glew.h>
#include <SDL_opengl.h>

namespace XR
{
namespace GL
{

GLint CheckError(char const* pText);

void SwitchEnabledState(GLenum name, bool state);

} // GL
} // XR

//==============================================================================
#define XR_GL_CALL(theCall)  theCall; XR::GL::CheckError( #theCall )

//==============================================================================
#define XR_GL_CALL_RESULT(theCall, result)\
  theCall;\
  result = XR::GL::CheckError( #theCall )

//==============================================================================
#define XR_GL_IGNORE_ERROR (void)glGetError();

//==============================================================================
// inline
//==============================================================================
namespace XR
{
namespace GL
{

//==============================================================================
inline
GLint  CheckError(char const* pText)
{
  GLint error = glGetError();
  if (error != GL_NO_ERROR)
  {
    XR_ERROR(("OpenGL error 0x%x occurred attempting '%s'.", error, pText));
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
