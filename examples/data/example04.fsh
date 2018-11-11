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
in vec4 vColor;

out vec4 FragColor;

void main()
{
  vec4 rgba = texture(uTexture0, vUv);
  rgba.rgb += vColor.rgb * vColor.a;
  FragColor = rgba;
}
