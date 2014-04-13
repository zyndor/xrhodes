//
// Nuclear Heart Games
// XRhodes
//
// maths.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    15/06/2011
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_MATHS_HPP
#define XR_MATHS_HPP

#include <cmath>
#include <stdlib.h>

//==============================================================================
#if !defined M_PI
#define M_PI   3.14159265358979323846
#endif

namespace XR
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

///@return  +1 is @a check is true, -1 otherwise.
template  <typename T>
T BoolToSign(bool check);

///@return  The smaller of two values.
template  <typename T>
T  Min(T a, T b);

///@return  The greater of two values.
template  <typename T>
T  Max(T a, T b);

///@return  The value @a val clamped to @a min and @a max.
template  <typename T>
T  Clamp(T val, T min, T max);

///@return  Per-frame speed value based on per second speed @a v and @a fps.
float CalcSpeedPerFrame(float v, float fps);

///@return  Per-frame acceleration value based on per second acceleration @a v
/// and @a fps.
float CalcAccelerationPerFrame(float v, float fps);

///@return  Per-frame scaling value based on per second scaling @a v and
/// @a fps.
float CalcScalingPerFrame(float v, float fps);

//==============================================================================
/*
  implementation
*/
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
  return (float)(rand() % RAND_MAX) * kRecRandMax;
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
T  Min(T a, T b)
{
  return a < b ? a : b;
}

//==============================================================================
template  <typename T>
inline
T  Max(T a, T b)
{
  return a > b ? a : b;
}

//==============================================================================
template  <typename T>
inline
T  Clamp(T val, T min, T max)
{
  return (val > min) ? ((val < max) ? val : max) : min;
}

}  // XR

#endif  //XR_MATHS_HPP
