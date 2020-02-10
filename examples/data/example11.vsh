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

in vec4 iData0;
in vec4 iData1;

#define INST_POS iData0.xyz
#define INST_SCALE iData0.w
#define INST_METAL_ROUGHNESS iData1.xy

out vec2 vUv;
out vec3 vColor;
out vec3 vViewPos;
out vec3 vNormal;
out vec3 vTangent;
out vec2 vMetalRoughness;

vec3 normalizeFast(vec3 v)
{
  return v * (1.5f - 5.f * dot(v, v));
}

vec3 getTangent(vec3 normal)
{
  vec3 t0 = cross(vNormal, vec3(1.f, 0.f, 0.f));
  vec3 t1 = cross(vNormal, vec3(0.f, 1.f, 0.f));
  float d0 = dot(t0, t0);
  float d1 = dot(t1, t1);
  return normalizeFast(t0 * step(d1, d0) + t1 * step(d0, d1));
}

void main()
{
  vec4 pos = xruModelView * vec4((aPosition * INST_SCALE) + INST_POS, 1.);
  vViewPos = pos.xyz;

  gl_Position = xruProjection * pos;
  vUv = aUV0;
  vColor = aColor0;

  vNormal = normalize(xruNormal * aPosition);
  vTangent = getTangent(vNormal);

  vMetalRoughness = INST_METAL_ROUGHNESS;
}
