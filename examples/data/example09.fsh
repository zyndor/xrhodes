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

uniform sampler2D uTexture0;
uniform sampler2D uShadowMap;
uniform vec4 uLightDir_InvShadowMapSize;
uniform vec4 uLightColor;

#define LIGHT_DIR uLightDir_InvShadowMapSize.xyz
#define INV_SHADOWMAP_SIZE uLightDir_InvShadowMapSize.w

uniform vec4 uNear_InvDepth;

#define NEAR uNear_InvDepth.x
#define INV_DEPTH uNear_InvDepth.y

in vec2 vUv;
in vec3 vColor;
in vec3 vNormal;
in vec3 vViewPos;
in vec3 vLightSpacePos;

const vec2 kOffsets[] = {
  vec2(0., 1.),
  vec2(.866, -.5),
  vec2(-.866, -.5),
};

float CalculateShadowPCF()
{
  float shadowAccum = 0.;
  for (int i = 0; i < 3; ++i)
  {
    float sample = texture(uShadowMap, vLightSpacePos.xy + kOffsets[i] * INV_SHADOWMAP_SIZE).r;
    float distance = vLightSpacePos.z;
    if (distance <= sample)
    {
      shadowAccum += 1.;
    }
  }
  
  return shadowAccum / 3.;
}

void main()
{
  vec3 normal = normalize(vNormal);
  vec3 viewPos = normalize(-vViewPos);

  vec3 invLightDir = -LIGHT_DIR;
  float lambertian = max(dot(normal, invLightDir), .0);

  vec3 lightDirReflected = reflect(LIGHT_DIR, normal);
  float specular = pow(max(dot(viewPos, lightDirReflected), .0), 256.0);

  vec4 rgba = texture(uTexture0, vUv);
  rgba.rgb += vColor * (1.0 - rgba.a) * .25;

  rgba.rgb *= .5 + uLightColor.rgb * lambertian;
  rgba.rgb += uLightColor.rgb * specular;

  float shadow = CalculateShadowPCF();
  rgba.rgb *= shadow;

  gl_FragColor = vec4(rgba.rgb, (NEAR + vViewPos.z) * -INV_DEPTH);
}
