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

in vec2 vUv;

out vec4 FragColor;

const float kRefValueLow = .2;
const float kRefValueHigh = .6;

void main()
{
  float component = texture(uTexture0, vUv).r;
  component = smoothstep(kRefValueLow, kRefValueHigh, component);

  float shadow = texture(uTexture0, vUv - vec2(1. / 256.)).r;
  shadow = smoothstep(kRefValueLow, kRefValueHigh, shadow);

  float shadowAlpha = shadow * (1. - component);

  FragColor = vec4(vec3(component), component + shadowAlpha);
}
