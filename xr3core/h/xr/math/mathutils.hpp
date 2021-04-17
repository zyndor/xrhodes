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
#include <cstdlib>
#include <algorithm>

//==============================================================================
#if !defined M_PI
#define M_PI   3.14159265358979323846
#endif

namespace xr
{

//==============================================================================
///@brief The reciprocal of kByteMax.
constexpr float kRecByteMax = 1.0f / 0xff;

///@brief kPi.
constexpr float kPi = float(M_PI);

///@brief Half of kPi.
constexpr float kPiHalf = kPi * .5f;

///@brief Double of kPi.
constexpr float kPi2 = kPi * 2.0f;

///@brief Square of kPiHalf.
constexpr float kPiHalfSqr = kPiHalf * kPiHalf;

///@brief An angleInDegrees * kDegreesToRadians is the angle in radians.
constexpr float kDegreesToRadians = kPi / 180.0f;

///@brief An angleInRadians * kRadiansToDegrees is the angle in degrees.
constexpr float kRadiansToDegrees = 1.0f / kDegreesToRadians;

///@brief Arbitrary small value for floating point difference calculations.
constexpr float kEpsilon = .00001f;

///@brief The reciprocal of RAND_MAX.
constexpr float kRecRandMax = 1.0f / float(RAND_MAX);

//==============================================================================
///@return +1 if true, -1 if false.
template <typename T>
constexpr int Sign(T t);

///@return the value rounded to the nearest integral value
[[deprecated("Use std::round().")]]
float Round(float f);

///@return An integer unit with a pseudo-random sign.
int RandSign();

///@return A pseudo-random floating point number in the [.0, 1.0] range.
float  FlopRand();

///@return +1 if @a check is true, -1 otherwise.
template <typename T>
constexpr T BoolToSign(bool check);

///@return The value @a val clamped to @a min and @a max.
template <typename T>
constexpr T  Clamp(T val, T min, T max);

///@return The value @a val clamped to @a min and @a max.
constexpr float Saturate(float val);

///@brief Performs linear interpolation.
///@return Value interpolated between @a v0 and @a v1, by @a blendFactor.
template <typename T>
constexpr T  Lerp(T v0, T v1, float blendFactor);

///@brief Performs the inverse of linear interpolation.
///@return The alpha value of @a value given edges @a edge0 and @a edge1.
template <typename T>
constexpr T  InvLerp(T edge0, T edge1, float value);

///@brief Calculates a value based on the given @a controlPoints and @a blendFactor
/// between .0 and 1.0.
template <typename T, size_t kNumSamples>
constexpr typename std::decay<T>::type Bezier(T const* controlPoints, float blendFactor);

///@brief Calculates a value based on the given @a controlPoints and @a blendFactor
/// between .0 and 1.0.
template <typename T, size_t kNumSamples>
constexpr typename std::decay<T>::type Bezier(T(&controlPoints)[kNumSamples], float blendFactor);

///@return	Interpolated value between @a edge0 and @a edge1.
float SmoothStep(float edge0, float edge1, float x);

///@return	Interpolated value between @a edge0 and @a edge1.
float SmootherStep(float edge0, float edge1, float x);

///@return Per-frame speed value based on per second speed @a v and @a fps.
float CalcSpeedPerFrame(float v, float fps);

///@return Per-frame acceleration value based on per second acceleration @a v
/// and @a fps.
float CalcAccelerationPerFrame(float v, float fps);

///@return Per-frame scaling value based on per second scaling @a v and
/// @a fps.
float CalcScalingPerFrame(float v, float fps);

///@brief Calculates the quadratic roots of @a a, @a b, @a c and stores them in
/// @a x0 and @a x1.
///@return Whether the quadratic has a solution.
///@note If the quadratic doesn't have a solution, the values of @a x0 and @a x1
/// will not be changed and should be ignored.
bool  SolveQuadratic(float a, float b, float c, float& x0, float& x1);

//==============================================================================
// implementation
//==============================================================================
template <typename T>
inline
constexpr int Sign(T t)
{
  return static_cast<int>((t > 0) - (t < 0));
}

//==============================================================================
inline
float Round(float f)
{
  return std::round(f);
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
template <typename T>
inline
constexpr T BoolToSign(bool check)
{
  return static_cast<T>((static_cast<int>(check) << 1) - 1);
}

//==============================================================================
template <typename T>
inline
constexpr T  Clamp(T val, T min, T max)
{
  return (val > min) ? ((val < max) ? val : max) : min;
}

//==============================================================================
inline
constexpr float Saturate(float val)
{
  return Clamp(val, 0.f, 1.f);
}

//==============================================================================
template <typename T>
inline
constexpr T Lerp(T v0, T v1, float blendFactor)
{
  return v0 + (v1 - v0) * blendFactor;
}

//==============================================================================
template <typename T>
inline
constexpr T InvLerp(T edge0, T edge1, float value)
{
  return (value - edge0) / (edge1 - edge0);
}

//==============================================================================
namespace detail
{

template <typename T, size_t kNumSamples>
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

template <typename T>
struct  BezierImpl<T, 1>
{
  static T Calculate(const T* parSamples, float blendFactor)
  {
    return parSamples[0];
  }
};

} // detail

template <typename T, size_t kNumSamples>
inline
constexpr typename std::decay<T>::type Bezier(T const* controlPoints, float blendFactor)
{
  return detail::BezierImpl<typename std::decay<T>::type, kNumSamples>::Calculate(controlPoints, blendFactor);
}

template <typename T, size_t kNumSamples>
inline
constexpr typename std::decay<T>::type Bezier(T (&controlPoints)[kNumSamples], float blendFactor)
{
  return detail::BezierImpl<typename std::decay<T>::type, kNumSamples>::Calculate(controlPoints, blendFactor);
}

//==============================================================================
inline
float SmoothStep(float edge0, float edge1, float x)
{
  x = Saturate(InvLerp(edge0, edge1, x));
  return x * x * (3.0f - 2.0f * x);
}

//==============================================================================
inline
float SmootherStep(float edge0, float edge1, float x)
{
  x = Saturate(InvLerp(edge0, edge1, x));
  return x * x * x * (x * (x * 6.0f - 15.0f) + 10.0f);
}

}  // xr

#endif  //XR_MATHS_HPP
