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
uniform vec4 uLightDir;
uniform vec4 uLightColor;

#define LIGHT_DIR uLightDir.xyz

in vec2 vUv;
in vec3 vColor;
in vec3 vNormal;
in vec3 vViewPos;

void main()
{
  // Renormalize normal, since this gets interpolated between fragment shader instances.
  vec3 normal = normalize(vNormal);
  vec3 viewPos = normalize(-vViewPos);

  vec3 invLightDir = -LIGHT_DIR;

  // Relative direction of surface to light.
  float lambertian = max(dot(normal, invLightDir), .0);

  // Reflect light off surface
  vec3 lightDirReflected = reflect(LIGHT_DIR, normal);

  // And calculate relative direction of reflected light and eye direction.
  float specular = pow(max(dot(viewPos, lightDirReflected), .0), 256.0);

  vec4 rgba = texture(uTexture0, vUv);
  rgba.rgb += vColor * (1.0 - rgba.a) * .25;

  // Add lighting to our sampled texture. Since the texture isn't that of some
  // realistic material, bump its RGB value down by 50% in the process.
  rgba.rgb *= .5 + uLightColor.rgb * lambertian;
  rgba.rgb += uLightColor.rgb * specular;

  gl_FragColor = rgba;
}
