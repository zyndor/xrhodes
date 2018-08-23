#ifndef XR_TWEENER_HPP
#define XR_TWEENER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================

#include <list>
#include "maths.hpp"
#include "xr/fundamentals.hpp"

namespace xr
{

//==============================================================================
///@brief Interpolates Float values over a duration to their given target,
/// using a function.
class  Tweener
{
  XR_NONCOPY_DECL(Tweener)

public:
  // types
  ///@return Interpolated value given initial @a v0 and target @a v1 values
  /// and blend factor @a t (.0...1.0).
  typedef float(*Function)(float v0, float v1, float t);

  ///@brief Callback for OnFrame and OnFinished events.
  typedef void(*Callback)(void*);

  // structors
  Tweener();
  ~Tweener();

  // general use
  void  Add(float duration, Function function, float target, float& value,
    Callback onFrame, Callback onFinished, void* data);
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
        return lhs.value < rhs.value;
      }
    };

    // data
    float     progress;
    float     invDuration;
    Function  function;
    float     vStart;
    float     vTarget;
    float*    value;
    Callback  onFrame;
    Callback  onFinished;
    void*     callbackData;

    // general use
    bool  Update(float tDelta);
  };

  typedef std::list<Param>  ParamList;

  // data
  ParamList  m_params;
};

}  // xr

#endif  //XR_TWEENER_HPP
