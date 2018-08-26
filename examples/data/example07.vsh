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
in vec2 aUV0;
in vec3 aColor0;
in vec3 aNormal;

in vec4 iData0;
in vec4 iData1;

#define INST_POS iData0.xyz
#define INST_SCALE iData0.w
#define INST_ROTATION iData1

out vec2 vUv;
out vec3 vColor;
out vec3 vViewPos;
out vec3 vNormal;

vec3 QuaternionRotate(vec4 q, vec3 p)
{
  return p + 2.0 * cross(q.xyz, cross(q.xyz, p) + q.w * p);
}

void main()
{
  vec4 pos = xruModelView * vec4((QuaternionRotate(INST_ROTATION, aPosition * INST_SCALE) + INST_POS), 1.);
  vViewPos = pos.xyz;

  gl_Position = xruProjection * pos;
  vUv = aUV0;
  vColor = aColor0;

  // Rotate normals by instance data.
  vec3 normal = QuaternionRotate(INST_ROTATION, aNormal);
  vNormal = normalize(xruNormal * normal);
}
