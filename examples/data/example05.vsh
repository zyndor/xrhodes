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

uniform mat4 xruModelViewProjection;

in vec3 aPosition;
in vec2 aUV0;

out vec2 vUv;

void main()
{
  vUv = aUV0;
  gl_Position = xruModelViewProjection * vec4(aPosition, 1.);
}
