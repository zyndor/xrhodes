//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#ifndef XR_TWEENER_HPP
#define XR_TWEENER_HPP

#include <list>
#include "maths.hpp"
#include "fundamentals.hpp"

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
  ///@return Interpolated value given initial @a v0 and target @a v1 values
  /// and blend factor @a t (.0...1.0).
  typedef float(*Function)(float v0, float v1, float t);
  
  ///@brief Callback for OnFrame and OnFinished events.
  typedef void(*Callback)(void*);

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
    float     progress;
    float     base;  // reciprocal of duration
    Function  pFunction;
    float     vStart;
    float     vTarget;
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

}  // XR

#endif  //XR_TWEENER_HPP
