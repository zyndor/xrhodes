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

uniform mat4 xruModelView;
uniform mat4 xruProjection;
uniform mat3 xruNormal;

in vec3 aPosition;

in vec4 iData0;
in vec4 iData1;

#define INST_POS iData0.xyz
#define INST_SCALE iData0.w
#define INST_ROTATION iData1

vec3 QuaternionRotate(vec4 q, vec3 p)
{
  return p + 2.0 * cross(cross(p, q.xyz) + q.w * p, q.xyz);
}

void main()
{
  vec4 pos = xruModelView * vec4((QuaternionRotate(INST_ROTATION, aPosition * INST_SCALE) + INST_POS) , 1.f);

  pos = xruProjection * pos;
  gl_Position = pos;
}
