//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#version 330

precision mediump float;

uniform mat4 xruModelViewProjection;  // uniform from xr::Transforms

in vec3 aPosition;
in vec2 aUV0;
in vec4 aColor0;

out vec2 vUv;
out vec4 vColor;

void main()
{
  vec4 pos = xruModelViewProjection * vec4(aPosition, 1.0);

  vUv = aUV0;
  vColor = aColor0;
  gl_Position = pos;
}
