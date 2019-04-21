//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#version 330

//#define DEBUG

precision mediump float;

uniform samplerCube uTextureCube;

in vec3 vUv;

out vec4 FragColor;

void main()
{
  vec4 rgba = texture(uTextureCube, vUv);
  
#ifdef DEBUG
  vec3 absUv = abs(vUv);
  float ma = max(absUv.x, max(absUv.y, absUv.z));
  vec3 col;
  if (ma == absUv.x)
  {
	col = vec3(1.0, vec2(vUv.z * ((float(absUv.x == vUv.x) * 2.) - 1.), -vUv.y) * .5 + .5);
  }
  else if(ma == absUv.y)
  {
	col = vec3(1.0, vec2(vUv.x, vUv.z * ((float(absUv.y == vUv.y) * 2.) - 1.)) * .5 + .5).yxz;
  }
  else
  {
	col = vec3(vec2(vUv.x * ((float(absUv.z == vUv.z) * 2.) - 1.), -vUv.y) * .5 + .5, 1.0);
  }
  
  rgba = vec4(col, 1.f) - fwidth(smoothstep(.3, .7, rgba));
#endif
  FragColor = rgba;
}
