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
uniform samplerCube uIrradiance;
uniform samplerCube uRadiance;

uniform vec4 uEnvRotation;
uniform vec4 uMaxLod;

#define MAX_LOD uMaxLod.x

in vec2 vUv;
in vec3 vColor;
in vec3 vNormal;
in vec3 vTangent;
in vec3 vViewPos;
in vec2 vMetalRoughness;

#define METALNESS vMetalRoughness.x
#define ROUGHNESS vMetalRoughness.y

out vec4 FragColor;

// https://www.unrealengine.com/blog/physically-based-shading-on-mobile
vec3 brdfApprox( vec3 specular, float roughness, float ndv )
{
  const vec4 c0 = vec4( -1.0, -0.0275, -0.572, 0.022 );
  const vec4 c1 = vec4( 1.0, 0.0425, 1.04, -0.04 );
  vec4 r = roughness * c0 + c1;
  float a004 = min( r.x * r.x, exp2( -9.28 * ndv ) ) * r.x + r.y;
  vec2 ab = vec2( -1.04, 1.04 ) * a004 + r.zw;

  return specular * ab.x + ab.y;
}

vec3 rotateByQuaternion(vec4 q, vec3 v)
{
  return v + 2.f * cross(q.xyz, cross(q.xyz, v) + v * q.w);
}

void main()
{
  vec3 normal = normalize(vNormal);
  vec3 tangent = normalize(vTangent);
  vec3 bitangent = cross(normal, tangent);
  mat3 tbn = mat3(tangent, bitangent, normal);

  vec3 texel = texture(uTexture0, vUv).rgb;
  vec3 albedo = mix(texel, vec3(1.f), .5f);
  vec3 normalMap = tbn * vec3(0.f, 0.f, 1.f);

  vec3 viewDir = normalize(vViewPos);
  vec3 irradiance = texture(uIrradiance, rotateByQuaternion(uEnvRotation, viewDir)).rgb;
  vec3 diffuse = albedo * irradiance * (1.f - METALNESS);

  vec3 viewDirRefl = reflect(viewDir, normalMap);
  float lod = mix(0.f, MAX_LOD, ROUGHNESS);
  vec3 radiance = textureLod(uRadiance, rotateByQuaternion(uEnvRotation, viewDirRefl), lod).rgb;
  vec3 specular = mix(vec3(.04f), albedo, METALNESS);
  float ndv = max(dot(normal, viewDir), 0.f);
  specular = radiance * brdfApprox(specular, ROUGHNESS, ndv);

  albedo = diffuse + specular;
  FragColor = vec4(albedo, 1.f);
}
