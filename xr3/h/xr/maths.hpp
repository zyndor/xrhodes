#ifndef XR_MATHS_HPP
#define XR_MATHS_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include <cmath>
#include <stdlib.h>
#include <algorithm>

//==============================================================================
#if !defined M_PI
#define M_PI   3.14159265358979323846
#endif

namespace xr
{

//==============================================================================
///@brief The reciprocal of kByteMax.
extern const float kRecByteMax;

///@brief kPi.
extern const float kPi;

///@brief Half of kPi.
extern const float kPiHalf;

///@brief Double of kPi.
extern const float kPi2;

///@brief Square of kPiHalf.
extern const float kPiHalfSqr;

///@brief An angleInDegrees * kDegreesToRadians is the angle in radians.
extern const float kDegreesToRadians;

///@brief An angleInRadians * kRadiansToDegrees is the angle in degrees.
extern const float kRadiansToDegrees;

///@brief Arbitrary small value for floating point difference calculations.
extern const float kEpsilon;

///@brief The reciprocal of RAND_MAX.
extern const float kRecRandMax;

//==============================================================================
///@return  +1 if true, -1 if false.
template  <typename T>
int Sign(T t);

///@return  the value rounded to the nearest integral value
float Round(float f);

///@return  An integer unit with a pseudo-random sign.
int RandSign();

///@return  A pseudo-random floating point number in the [.0, 1.0] range.
float  FlopRand();

///@return  +1 if @a check is true, -1 otherwise.
template  <typename T>
T BoolToSign(bool check);

///@return  The value @a val clamped to @a min and @a max.
template  <typename T>
T  Clamp(T val, T min, T max);

///@return  The value @a val clamped to @a min and @a max.
float Saturate(float val);

///@return  Linear interpolated value between @a v0 and @A v1 at @a blendFactor.
template  <typename T>
T  Lerp(T v0, T v1, float blendFactor);

///@brief Calculates a value based on the given @a controlPoints and @a blendFactor
/// between .0 and 1.0.
template  <typename T, size_t N>
T Bezier(T* const controlPoints, float blendFactor);

///@return	Interpolated value between @a edge0 and @a edge1.
float SmoothStep(float edge0, float edge1, float x);

///@return	Interpolated value between @a edge0 and @a edge1.
float SmootherStep(float edge0, float edge1, float x);

///@return  Per-frame speed value based on per second speed @a v and @a fps.
float CalcSpeedPerFrame(float v, float fps);

///@return  Per-frame acceleration value based on per second acceleration @a v
/// and @a fps.
float CalcAccelerationPerFrame(float v, float fps);

///@return  Per-frame scaling value based on per second scaling @a v and
/// @a fps.
float CalcScalingPerFrame(float v, float fps);

///@brief Calculates the quadratic roots of @a a, @a b, @a c and stores them in
/// @a x0 and @a x1.
///@return  Whether the quadratic has a solution.
///@note If the quadratic doesn't have a solution, the values of @a x0 and @a x1
/// will not be changed and should be ignored.
bool  SolveQuadratic(float a, float b, float c, float& x0, float& x1);

//==============================================================================
// implementation
//==============================================================================
template  <typename T>
inline
int Sign(T t)
{
  return static_cast<int>((t > 0) - (t < 0));
}

//==============================================================================
inline
float Round(float f)
{
  return floorf(f + .5f);
}

//==============================================================================
inline
int RandSign()
{
  return ((rand() & 1) << 1) - 1;
}

//==============================================================================
inline
float  FlopRand()
{
  return float(rand()) * kRecRandMax;
}

//==============================================================================
template  <typename T>
inline
T BoolToSign(bool check)
{
  return static_cast<T>((static_cast<int>(check) << 1) - 1);
}

//==============================================================================
template  <typename T>
inline
T  Clamp(T val, T min, T max)
{
  return (val > min) ? ((val < max) ? val : max) : min;
}

//==============================================================================
inline
float Saturate(float val)
{
  return val < 1.0f ? (val > .0f ? val : .0f) : 1.0f;
}

//==============================================================================
template  <typename T>
inline
T Lerp(T v0, T v1, float blendFactor)
{
  return v0 + (v1 - v0) * blendFactor;
}

//==============================================================================
template  <typename T, size_t kNumSamples>
struct  BezierImpl
{
  static_assert(kNumSamples > 0, "Invalid number of samples.B");

  static T Calculate(const T* controlPoints, float blendFactor)
  {
    T results[kNumSamples - 1];
    for(int i = 1; i < kNumSamples; ++i)
    {
      const size_t iLess1 = i - 1;
      results[iLess1] = Lerp(controlPoints[iLess1], controlPoints[i], blendFactor);
    }
    return BezierImpl<T, kNumSamples - 1>::Calculate(results, blendFactor);
  }
};

template  <typename T>
struct  BezierImpl<T, 1>
{
  static T Calculate(const T* parSamples, float blendFactor)
  {
    return parSamples[0];
  }
};

template  <typename T, size_t kNumSamples>
inline
T Bezier(T const* controlPoints, float blendFactor)
{
  return BezierImpl<T, kNumSamples>::Calculate(controlPoints, blendFactor);
}

//==============================================================================
inline
float SmoothStep(float edge0, float edge1, float x)
{
  x = Saturate((x - edge0) / (edge1 - edge0));
  return x * x * (3.0f - 2.0f * x);
}

//==============================================================================
inline
float SmootherStep(float edge0, float edge1, float x)
{
  x = Saturate((x - edge0) / (edge1 - edge0));
  return x * x * x * (x * (x * 6.0f - 15.0f) + 10.0f);
}

}  // xr

#endif  //XR_MATHS_HPP
