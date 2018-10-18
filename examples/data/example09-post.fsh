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

uniform vec4 uFocalLength_Amount_InvAspectRatio;

#define FOCAL_LENGTH uFocalLength_Amount_InvAspectRatio.x
#define BOKEH_AMOUNT uFocalLength_Amount_InvAspectRatio.z
#define INV_ASPECT_RATIO uFocalLength_Amount_InvAspectRatio.w

#define PI 3.141596
#define GOLDEN_ANGLE 7.19996323

in vec2 vUv;

// Calculate sine and cosine of golden angle up front if possible.
const float kSinGolden = sin(GOLDEN_ANGLE);
const float kCosGolden = cos(GOLDEN_ANGLE);

vec3 Pow8(vec3 v)
{
    v *= v;
    v *= v;
    v *= v;
    return vec3(v);
}

void main(void)
{
  // Determine distance from focal point; calculate bias.
  float z = texture(uTexture0, vUv).a;
  float bias = abs(FOCAL_LENGTH - z) * 2.;
  bias *= smoothstep(.0, .0625, bias);

  // Get (delta) uv to neighbor to blur with.
  vec2 texel = vec2(INV_ASPECT_RATIO, 1.) * bias * .025;

  // We will sample neighboring texels using a rotated coordinate.
  vec2 sample = vec2(0., 1.);
  float r = 1.;

  vec3 acc = vec3(0.);
  vec3 div = vec3(0.);
  for (int i = 0; i < 30; ++i)
  {
    r += 1. / r;  // Create a spiral by increasing the radius.

    vec2 newSample = vec2(sample.x * kCosGolden - sample.y * kSinGolden,
    sample.x * kSinGolden + sample.y * kCosGolden);
    sample = newSample;

    // Sample and accumulate, weighed by bokeh.
    vec3 col = texture(uTexture0, vUv + texel * newSample * (r - 1.) * .25).rgb;
    vec3 bokeh = vec3(.5) + Pow8(col) * BOKEH_AMOUNT;
    acc += col * bokeh;
    div += bokeh;
  }

  acc /= div;

  gl_FragColor = vec4(acc, 1.);
}
