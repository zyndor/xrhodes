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

in vec2 aPosition;
in vec2 aUV0;

out vec2 vUv;

void main()
{
  vUv = aUV0;
  gl_Position = vec4(aPosition, 0.f, 1.0);
}
