//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#version 330

#define USE_SDF

precision mediump float;

uniform sampler2D uTexture0;

in vec2 vUv;

out vec4 FragColor;

#ifdef USE_SDF
const float kRefValueLow = .2;
const float kRefValueHigh = .6;
#endif

void main()
{
  float component = texture(uTexture0, vUv).r;
#ifdef USE_SDF
  component = smoothstep(kRefValueLow, kRefValueHigh, component);

  float shadow = texture(uTexture0, vUv - vec2(1. / 256.)).r;
  shadow = smoothstep(kRefValueLow, kRefValueHigh, shadow);

  float shadowAlpha = shadow * (1. - component);

  FragColor = vec4(vec3(component), component + shadowAlpha);
#else
  FragColor = vec4(component);	// no shadows with bitmap fonts - bitmaps would need to be expanded to allow trivial UV offsetting.
#endif
}
