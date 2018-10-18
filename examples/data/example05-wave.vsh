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
uniform vec4 uTime;

in vec3 aPosition;
in vec2 aUV0;

out vec2 vUv;

void main()
{
  vUv = aUV0;
  
  float time = uTime.x * 0.005;
  vec4 position = vec4(aPosition.x + sin(time + aPosition.y * .17),
  aPosition.y + sin(time + aPosition.x * .31),
  aPosition.z + sin(time + aPosition.z * .53),
  1.);
  gl_Position = xruModelViewProjection * position;
}
