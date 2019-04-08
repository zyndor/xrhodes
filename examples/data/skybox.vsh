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

out vec3 vUv;

void main()
{
  vec4 position = xruModelViewProjection * vec4(aPosition, 0.f);
  gl_Position = position.xyww;
  
  vUv = aPosition;
}
