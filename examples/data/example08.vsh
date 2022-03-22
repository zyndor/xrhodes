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

uniform mat4 xruModel;
uniform mat4 xruView;
uniform mat4 xruProjection;
uniform mat3 xruNormal;

uniform mat4 uLightSpaceTransform;
uniform vec4 uLightDir_InvShadowMapSize;

#define LIGHT_DIR uLightDir_InvShadowMapSize.xyz

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
out vec3 vLightSpacePos;

vec3 QuaternionRotate(vec4 q, vec3 p)
{
  return p + 2.0 * cross(cross(p, q.xyz) + q.w * p, q.xyz);
}

void main()
{
  vec4 pos = xruModel * vec4((QuaternionRotate(INST_ROTATION, aPosition * INST_SCALE) + INST_POS), 1.);
  vec4 viewPos = xruView * pos;

  gl_Position = xruProjection * viewPos;
  vUv = aUV0;
  vColor = aColor0;
  vViewPos = viewPos.xyz;

  vec3 normal = QuaternionRotate(INST_ROTATION, aNormal);
  vNormal = normalize(xruNormal * normal);

  vec4 lightSpacePos = uLightSpaceTransform * pos;
  vLightSpacePos.xy = .5 * (lightSpacePos.xy + lightSpacePos.ww);
  
  float shadowBias = max(tan(dot(vNormal, -LIGHT_DIR)) * 0.016525, 0.);
  vLightSpacePos.z = .5 * (lightSpacePos.z + lightSpacePos.w - shadowBias);
}
