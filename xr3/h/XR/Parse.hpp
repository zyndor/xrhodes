#ifndef XR_PARSE_HPP
#define XR_PARSE_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================

#include "Color.hpp"
#include "XR/fundamentals.hpp"

namespace XR
{

//==============================================================================
class Parse
{
  XR_NONOBJECT_DECL(Parse)

public:
  // types
  enum  Error
  {
    ERR_NONE,
    ERR_NULL_STRING,
    ERR_INVALID_VALUE,
    kNumErrors
  };

  // static
  static const char* const  karErrorMessage[kNumErrors];
  
  ///@return error code from last operation.
  static int  GetErrorCode();

  ///@brief Parses a string for a boolean, defaulting to positive unless <= 0,
  /// n{...} or N{...} is found. The string is allowed to be null.
  ///@return  Boolean value found (or defaulted to).
  static bool BooleanPositive(const char* pValue);

  ///@brief Parses a string for a boolean, defaulting to negative unless > 0,
  /// y{...} or Y{...} is found. The string is allowed to be null.
  ///@return  Boolean value found (or defaulted to).
  static bool BooleanNegative(const char* pValue);

  ///@brief Parses a string for an integer, defaulting to 0.
  ///@return  Integer value found (or defaulted to).
  static int  Int(const char* pValue);

  ///@brief Parses a string for an integer, defaulting to 0. If a value with
  /// '%' or 'x' is specified or is floating point (or a combination of the
  /// above), then it will be used to scale base with. 
  ///@return  Integer value found (or defaulted to).
  static int  Int(int base, const char* pValue);

  ///@brief Parses a string for an int value. If the operation was successful,
  /// the value is then assigned to @a v.
  ///@return  Whether the operation was successful.
  static bool Int(const char* pValue, int& v);

  ///@brief Parses a string for an int value. If a value with '%' or 'x' is
  /// specified or is floating point (or a combination of the above), then it
  /// will be used to scale base with. If the operation was successful, the
  /// value is then assigned to @a v.
  ///@return  Whether the operation was successful.
  static bool Int(int base, const char* pValue, int& v);

  ///@brief Parses a string for a float value, defaulting to .0f. If a value
  /// with '%' is specified, then 1/100 of it is used. If the operation was
  /// successful, the value is then assigned to @a v.
  ///@return  Float value found (or defaulted to).
  static float  Float(const char* pValue);
  
  ///@brief Parses a string for a float value. If a value with '%' is
  /// specified, then 1/100 of it is used. If the operation was successful, the
  /// value is then assigned to @a v.
  ///@return  Whether the operation was successful.
  static bool Float(const char* pValue, float& v);

  ///@brief Parses a string for a float value to be used as a per second speed
  /// value. If the operation was successful, the value is then assigned to
  /// @a v.
  ///@return  Whether the operation was successful.
  static bool FloatSpeedPerSecond(const char* pValue, float fps, float& v);

  ///@brief Parses a string for a float value to be used as a per second
  /// acceleration value. If the operation was successful, the value is then
  /// assigned to @a v.
  ///@return  Whether the operation was successful.
  static bool FloatAccelerationPerSecond(const char* pValue, float fps, float& v);

  ///@brief Parses a string for a float value to be used as a per second
  /// scaling value. If the operation was successful, the value is then
  /// assigned to @a v.
  ///@return  Whether the operation was successful.
  static bool FloatScalingPerSecond(const char* pValue, float fps, float& v);

  ///@brief Parses a string for two ends of a range, separated by a hyphen.
  ///@return  Whether the operation was successful.
  static bool Range(const char* pValue, int& r0, int& r1);

  ///@brief Parses a string for two ends of a range, separated by a hyphen.
  ///@return  Whether the operation was successful.
  static bool Range(const char* pValue, float& r0, float& r1);

  ///@brief Parses a string for an RGBA color, defaults to transparent black.
  ///@return  Color found (or defaulted to).
  static XR::Color Color(const char* pValue);

protected:
  // static
  static int  s_errorCode;
};

} // XR

#endif  //XR_PARSE_HPP