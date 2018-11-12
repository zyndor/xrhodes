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

void main()
{
  vec4 rgba = texture(uTexture0, vUv);
  rgba.a *= .5;
  FragColor = rgba;
}
