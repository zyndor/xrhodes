//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
//==============================================================================
#include <stdio.h>
#include "XR/Parse.hpp"
#include "XR/maths.hpp"

namespace XR
{

//==============================================================================
const char* const Parse::karErrorMessage[] =
{
  "OK",
  "Failed to parse null string.",
  "Failed to parse '%s' for %s value",
};

int Parse::s_errorCode(ERR_NONE);

//==============================================================================
int Parse::GetErrorCode()
{
  return s_errorCode;
}

//==============================================================================
bool Parse::BooleanPositive( const char* pValue )
{
  s_errorCode = ERR_NONE;

  bool  result(true);
  if (pValue != 0 && pValue[0] != '\0')
  {
    int dummy;
    if ((sscanf(pValue, "%d", &dummy) > 0 && dummy == 0) ||
      pValue[0] == 'n' || pValue[0] == 'N')
    {
      result = false;
    }
  }
  return result;
}

//==============================================================================
bool Parse::BooleanNegative( const char* pValue )
{
  s_errorCode = ERR_NONE;

  bool  result(false);
  if (pValue != 0 && pValue[0] != '\0')
  {
    int dummy;
    if ((sscanf(pValue, "%d", &dummy) > 0 && dummy > 0) ||
      pValue[0] == 'y' || pValue[0] == 'Y')
    {
      result = true;
    }
  }
  return result;
}

//==============================================================================
int   Parse::Int(const char* pValue)
{
  int v;
  if (!Int(pValue, v))
  {
    v = 0;
  }
  s_errorCode = ERR_NONE;
  return v;
}

//==============================================================================
int   Parse::Int(int base, const char* pValue)
{
  int v;
  if (!Int(base, pValue, v))
  {
    v = 0;
  }
  s_errorCode = ERR_NONE;
  return v;
}

//==============================================================================
bool  Parse::Int( const char* pValue, int& v)
{
  s_errorCode = ERR_NONE;

  bool  success(pValue != 0);
  if (!success)
  {
    s_errorCode = ERR_NULL_STRING;
  }

  int value(0);
  if (success)
  {
    success = sscanf(pValue, "%d", &value) == 1;
    if (!success)
    {
      s_errorCode = ERR_INVALID_VALUE;
    }
  }

  if (success)
  {
    v  = value;
  }
  return success;
}

//==============================================================================
bool  Parse::Int(int base, const char* pValue, int& v)
{
  s_errorCode = ERR_NONE;

  bool  success(pValue != 0);
  if (!success)
  {
    s_errorCode = ERR_NULL_STRING;
  }

  int value(0);
  if (success)
  {
    if (strchr(pValue, '%') != 0)
    {
      double  percent;
      success = sscanf(pValue, "%lf", &percent) == 1;
      if (!success)
      {
        s_errorCode = ERR_INVALID_VALUE;
      }

      if (success)
      {
        value = static_cast<int>(Round(float(percent * base / 100.0)));
      }
    }
    else if (strchr(pValue, 'x') != 0)
    {
      double  multi;
      const char* pTemp(pValue);
      if (pTemp[0] == 'x')
      {
        ++pTemp;
      }

      success = sscanf(pTemp, "%lf", &multi) == 1;
      if (!success)
      {
        s_errorCode = ERR_INVALID_VALUE;
      }

      if (success)
      {
        value = static_cast<int>(Round(float(multi * base)));
      }
    }
    else if (strchr(pValue, '.') != 0)
    {
      double  scalar;
      success = sscanf(pValue, "%lf", &scalar) == 1;
      if (!success)
      {
        s_errorCode = ERR_INVALID_VALUE;
      }

      if (success)
      {
        value = static_cast<int>(Round(float(scalar * base)));
      }
    }
    else
    {
      success = sscanf(pValue, "%d", &value) == 1;
      if (!success)
      {
        s_errorCode = ERR_INVALID_VALUE;
      }
    }
  }

  v = value;
  return success;
}

//==============================================================================
float Parse::Float(const char* pValue)
{
  float v;
  if (!Float(pValue, v))
  {
    v = .0f;
  }
  s_errorCode = ERR_NONE;
  return v;
}

//==============================================================================
bool  Parse::Float(const char* pValue, float& v)
{
  s_errorCode = ERR_NONE;

  bool  success(pValue != 0);
  if (!success)
  {
    s_errorCode = ERR_NULL_STRING;
  }

  float value(.0f);
  if (success)
  {
    if (strchr(pValue, '%') != 0)
    {
      double  percent;
      success = sscanf(pValue, "%lf", &percent) == 1;
      if (!success)
      {
        s_errorCode = ERR_INVALID_VALUE;
      }

      if (success)
      {
        value = float(percent / 100.0);
      }
    }
    else
    {
      success = sscanf(pValue, "%f", &value) == 1;
      if (!success)
      {
        s_errorCode = ERR_INVALID_VALUE;
      }
    }
  }

  v = value;
  return success;
}

//==============================================================================
bool  Parse::FloatSpeedPerSecond(const char* pValue, float fps, float& v)
{
  s_errorCode = ERR_NONE;

  XR_ASSERT(Parse::FloatSpeedPerSecond, fps > .0f);
  bool  success(pValue != 0);
  if (!success)
  {
    s_errorCode = ERR_NULL_STRING;
  }

  float value(.0f);
  if (success)
  {
    success = sscanf(pValue, "%f", &value) == 1;
    if (!success)
    {
      s_errorCode = ERR_INVALID_VALUE;
    }

    if (success)
    {
      value = CalcSpeedPerFrame(value, fps);
    }
  }

  v = value;
  return success;
}

//==============================================================================
bool  Parse::FloatAccelerationPerSecond(const char* pValue, float fps, float& v)
{
  s_errorCode = ERR_NONE;

  XR_ASSERT(Parse::FloatAccelerationPerSecond, fps > .0f);
  bool  success(pValue != 0);
  if (!success)
  {
    s_errorCode = ERR_NULL_STRING;
  }

  float value(.0f);
  if (success)
  {
    success = sscanf(pValue, "%f", &value) == 1;
    if (!success)
    {
      s_errorCode = ERR_INVALID_VALUE;
    }

    if (success)
    {
      value = CalcAccelerationPerFrame(value, fps);
    }
  }

  v = value;
  return success;
}

//==============================================================================
bool  Parse::FloatScalingPerSecond(const char* pValue, float fps, float& v)
{
  s_errorCode = ERR_NONE;

  XR_ASSERT(Parse::FloatScalingPerSecond, fps > .0f);
  bool  success(pValue != 0);
  if (!success)
  {
    s_errorCode = ERR_NULL_STRING;
  }

  float value(.0f);
  if (success)
  {
    success = sscanf(pValue, "%f", &value) == 1;
    if (!success)
    {
      s_errorCode = ERR_INVALID_VALUE;
    }

    if (success)
    {
      value = CalcScalingPerFrame(value, fps);
    }
  }

  v = value;
  return success;
}

//==============================================================================
bool Parse::Range( const char* pValue, int& r0, int& r1 )
{
  s_errorCode = ERR_NONE;

  bool  success(pValue != 0);
  if (!success)
  {
    s_errorCode = ERR_NULL_STRING;
  }

  r0 = r1 = 0;
  if (success)
  {
    size_t len = strlen(pValue);
    success = len > 0 && sscanf(pValue, "%d", &r0) == 1;
    if (!success)
    {
      s_errorCode = ERR_INVALID_VALUE;
    }

    if (success)
    {
      const char* pEnd(pValue + len);
      pValue = strchr(pValue, '-');
      if (pValue != 0 && r0 < 0)
      {
        pValue = strchr(pValue + 1, '-');
      }

      if (pValue == 0)
      {
        r1 = r0;
      }
      else
      {
        ++pValue;
        success = pValue != pEnd && sscanf(pValue, "%d", &r1) == 1 && r1 >= r0;
      }
    }
  }
  return success;
}

//==============================================================================
bool Parse::Range( const char* pValue, float& r0, float& r1 )
{
  s_errorCode = ERR_NONE;

  bool  success(pValue != 0);
  if (!success)
  {
    s_errorCode = ERR_NULL_STRING;
  }

  r0 = r1 = 0;
  if (success)
  {
    size_t len = strlen(pValue);
    success = len > 0 && sscanf(pValue, "%f", &r0) == 1;
    if (!success)
    {
      s_errorCode = ERR_INVALID_VALUE;
    }

    if (success)
    {
      const char* pEnd(pValue + len);
      pValue = strchr(pValue, '-');
      if (pValue != 0 && r0 < .0f)
      {
        pValue = strchr(pValue + 1, '-');
      }

      if (pValue == 0)
      {
        r1 = r0;
      }
      else
      {
        ++pValue;
        success = pValue != pEnd && sscanf(pValue, "%f", &r1) == 1 && r1 >= r0;
      }
    }
  }
  return success;
}

//==============================================================================
XR::Color Parse::Color( const char* pValue )
{
  s_errorCode = ERR_NONE;

  XR::Color col(0xffffffff);
  if (pValue != 0)
  {
    uint32_t  rgba;
    if (sscanf(pValue, "%x", &rgba) > 0)
    {
      if (strlen(pValue) <= 6)
      {
        rgba <<= 8;
        col.a = 1.0f;
      }
      else
      {
        col.a = float(XR_RGBA_EXTRACT_ALPHA(rgba) / 255.0f);
      }

      col.r = float(XR_RGBA_EXTRACT_RED(rgba) / 255.0f);
      col.g = float(XR_RGBA_EXTRACT_GREEN(rgba) / 255.0f);
      col.b = float(XR_RGBA_EXTRACT_BLUE(rgba) / 255.0f);
    }
  }
  return col;
}

} // XR
