//
// Nuclear Heart Games
// XRhodes
//
// Tweener.hpp
// @author  Gyorgy Straub <gyorgy@nuclearheart.com>
// @date    12/07/2011
//
// copyright (c) 2011 - 2014. All rights reserved.
//
//==============================================================================
#if !defined XR_TWEENER_HPP
#define XR_TWEENER_HPP

#include <list>
#include "maths.hpp"
#include "types.hpp"

namespace XR
{

//==============================================================================
///@brief Interpolates Float values over a duration to their given target,
/// using a function.
class  Tweener
{
  XR_NONCOPY_DECL(Tweener)

public:
  // types
  ///@brief Function for tweening. @a t is the remaining lifespan of the
  /// parameter, @a t1 is the reciprocal of the parameter's duration (@a t *
  /// @a t1 is the remaining percentage), @a v1 is the target value, @a dv
  /// is the change in the value (@a v1 + @a dv is the initial value).
  typedef float(*Function)(float t, float t1, float v1, float dv);
  typedef void(*Callback)(void*);

  // static
  static float  Linear(float t, float t1, float v1, float dv);
  static float  QuadIn(float t, float t1, float v1, float dv);
  static float  QuadOut(float t, float t1, float v1, float dv);
  static float  QuadInOut(float t, float t1, float v1, float dv);
  static float  CubeIn(float t, float t1, float v1, float dv);
  static float  CubeOut(float t, float t1, float v1, float dv);
  static float  CubeInOut(float t, float t1, float v1, float dv);

  template  <int kN>
  static float  NthIn(float t, float t1, float v1, float dv);

  template  <int kN>
  static float  NthOut(float t, float t1, float v1, float dv);

  template  <int kN>
  static float  NthInOut(float t, float t1, float v1, float dv);

  static float  SinIn(float t, float t1, float v1, float dv);
  static float  SinOut(float t, float t1, float v1, float dv);
  static float  SinInOut(float t, float t1, float v1, float dv);
  static float  SinOutIn(float t, float t1, float v1, float dv);

  static float BounceIn(float t, float t1, float v1, float dv);
  static float BounceOut(float t, float t1, float v1, float dv);
  static float BounceInOut(float t, float t1, float v1, float dv);
  
  static float ElasticIn(float t, float t1, float v1, float dv);
  static float ElasticOut(float t, float t1, float v1, float dv);
  // exponential, circular?

  // structors
  Tweener();
  ~Tweener();
  
  // general use
  void  Add(float duration, Function pFunction, float target, float& value,
    Callback pOnFrame, Callback pOnFinished, void* pData);
  bool  Remove(float& value, bool finish);
  void  Update(float tDelta);

  void  Clear();

private:
  // typedef
  struct  Param
  {
    // types
    struct  ComparePredicate
    {
      bool  operator()(const Param& lhs, const Param& rhs) const
      {
        return lhs.pValue < rhs.pValue;
      }
    };
    
    // data
    float     lifespan;
    float     duration;  // stored as reciprocal. lifespan * duration = remaining% (0...1)
    Function  pFunction;
    float     target;  // the value at lifespan = .0
    float     delta;  // = target - initial value
    float*    pValue;
    Callback  pOnFrame;
    Callback  pOnFinished;
    void*     pCallbackData;

    // general use
    bool  Update(float tDelta);
  };

  typedef std::list<Param>  ParamList;

  // data
  ParamList  m_params;
};

//==============================================================================
// implementation
//==============================================================================
///@brief Linear tweener.
inline
float  Tweener::Linear(float t, float t1, float v1, float dv)
{
  return v1 + (dv * t * t1);
}

//==============================================================================
///@brief Quadratic easing in tweener.
inline
float  Tweener::QuadIn(float t, float t1, float v1, float dv)
{
  t = 1.0f - (t * t1);
  return v1 + dv - (dv * t * t);
}

//==============================================================================
///@brief Quadratic easing out tweener.
inline
float  Tweener::QuadOut(float t, float t1, float v1, float dv)
{
  t *= t1;
  return v1 + (dv * t * t);
}

//==============================================================================
///@brief Quadratic easing in & out tweener.
inline
float  Tweener::QuadInOut(float t, float t1, float v1, float dv)
{
  t *= t1 * 2.0f;
  if (t < 1.0f)
  {
    return v1 + (dv * t * t * .5f);
  }
  else
  {
    t = 2.0f - t;
    return v1 + dv - (dv * t * t * .5f);
  }
}

//==============================================================================
///@brief Cubic easing in tweener.
inline
float  Tweener::CubeIn(float t, float t1, float v1, float dv)
{
  t = 1.0f - (t * t1);
  return v1 + dv - (dv * t * t * t);
}

//==============================================================================
///@brief Cubic easing out tweener.
inline
float  Tweener::CubeOut(float t, float t1, float v1, float dv)
{
  t *= t1;
  return v1 + (dv * t * t * t);
}

//==============================================================================
///@brief Cubic easing in & out tweener.
inline
float  Tweener::CubeInOut(float t, float t1, float v1, float dv)
{
  t *= t1 * 2.0f;
  if (t < 1.0f)
  {
    return v1 + (dv * t * t * t * .5f);
  }
  else
  {
    t = 2.0f - t;
    return v1 + dv - (dv * t * t * t * .5f);
  }
}

//==============================================================================
///@brief Nth-power easing in tweener template.
template  <int kN>
inline
float  Tweener::NthIn(float t, float t1, float v1, float dv)
{
  t = 1.0f - (t * t1);
  return v1 + dv - (dv * powf(t, (float)(kN)));
}

//==============================================================================
///@brief Nth-power easing out tweener template.
template  <int kN>
inline
float  Tweener::NthOut(float t, float t1, float v1, float dv)
{
  t *= t1;
  return v1 + (dv * powf(t, float(kN)));
}

//==============================================================================
///@brief Nth-power easing in & out tweener template.
template  <int kN>
inline
float  Tweener::NthInOut(float t, float t1, float v1, float dv)
{
  t *= t1 * 2.0f;
  if (t < 1.0f)
  {
    return v1 + (dv * powf(t, float(kN)) * .5f);
  }
  else
  {
    return v1 + dv - (dv * powf(2.0f - t, (float)(kN)) * .5f);
  }
}

//==============================================================================
///@brief Sine easing in tweener.
inline
float  Tweener::SinIn(float t, float t1, float v1, float dv)
{
  return v1 + (dv * sinf(t * t1 * kPiHalf));
}

//==============================================================================
///@brief Sine easing out tweener.
inline
float  Tweener::SinOut(float t, float t1, float v1, float dv)
{
  return v1 + dv - (dv * sinf((1.0f - (t * t1)) * kPiHalf));
}

//==============================================================================
///@brief Sine easing in & out tweener.
inline
float  Tweener::SinInOut(float t, float t1, float v1, float dv)
{
  t *= t1;
  return v1 + (dv * (sinf(t * kPi - kPiHalf) * .5f + .5f));
}

//==============================================================================
///@brief Sine easing out & in tweener.
inline
float  Tweener::SinOutIn(float t, float t1, float v1, float dv)
{
  t *= t1 * 2.0f;
  if (t < 1.0f)
  {
    return v1 + (dv * sinf(t * kPiHalf) * .5f);
  }
  else
  {
    return v1 + dv - (dv * sinf(t * kPiHalf) * .5f);
  }
}

//==============================================================================
static const float kBounceMagic = 13.0f;

//==============================================================================
///@brief Bounce in tweener.
inline
float Tweener::BounceIn(float t, float t1, float v1, float dv)
{
  t *= t1;
  return v1 + dv * (1.0f - (1.0f - t) * fabsf(cosf(t * t * kBounceMagic)));
}

//==============================================================================
///@brief Bounce out tweener.
inline
float Tweener::BounceOut(float t, float t1, float v1, float dv)
{
  t *= t1;
  float t2((1.0f - t));
  return v1 + dv * t * fabsf(cosf(t2 * t2 * kBounceMagic));
}

//==============================================================================
///@brief Bounce in & out tweener.
inline
float Tweener::BounceInOut(float t, float t1, float v1, float dv)
{
  t *= t1;
  if (t < .5f)  
  {
    float t2((1.0f - t));
    return v1 + dv * t * fabsf(cosf(t2 * t2 * kBounceMagic));
  }
  else
  {
    return v1 + dv * (1.0f - (1.0f - t) * fabsf(cosf(t * t * kBounceMagic)));
  }
}

//==============================================================================
const float  kElasticAmplitude = 1.0625f;
const float  kElasticPeriod = .25f;

//==============================================================================
///@brief Elastic in tweener.
inline
float Tweener::ElasticIn(float t, float t1, float v1, float dv)
{
  t *= t1;
  t1 = kElasticAmplitude / t1;

  return v1 + dv - (dv * kElasticAmplitude * powf(2, -10.0f * t) * sinf(((t1 *
    kElasticPeriod) - t) * kPi2 / t1));
} 

//==============================================================================
///@brief Elastic out tweener.
inline
float Tweener::ElasticOut(float t, float t1, float v1, float dv)
{
  t *= t1;
  t1 = kElasticAmplitude / t1;

  return (v1 - dv * kElasticAmplitude * powf(2, 10.0f * (t - 1.0f)) *
    sinf(((t1 * kElasticPeriod) - t) * kPi2 / t1));
}

}  // XR

#endif  //XR_TWEENER_HPP
