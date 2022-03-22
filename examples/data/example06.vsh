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
in vec4 aColor0;

in vec4 iData0;  // instance data from Gfx
in vec4 iData1;

#define INST_POS iData0.xyz
#define INST_SCALE iData0.w
#define INST_ROTATION iData1

out vec2 vUv;
out vec4 vColor;

vec3 QuaternionRotate(vec4 q, vec3 p)
{
  return p + 2.0 * cross(cross(p, q.xyz) + q.w * p, q.xyz);
}

void main()
{
  // scale, rotate, then translate the vertex position, in this order.
  vec4 pos = xruModelViewProjection * vec4((QuaternionRotate(INST_ROTATION, aPosition * INST_SCALE) + INST_POS), 1.f);

  vUv = aUV0;
  vColor = aColor0;
  gl_Position = pos;
}
