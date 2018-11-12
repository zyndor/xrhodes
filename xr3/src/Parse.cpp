//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Parse.hpp"
#include "xr/math/mathutils.hpp"
#include <cstdio>
#include <cstring>

namespace xr
{

//==============================================================================
const char* const Parse::kErrorMessages[] =
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
bool Parse::BooleanPositive( const char* value )
{
  s_errorCode = ERR_NONE;

  bool  result = true;
  if (value != nullptr && value[0] != '\0')
  {
    int dummy;
    if ((sscanf(value, "%d", &dummy) > 0 && dummy == 0) ||
      value[0] == 'n' || value[0] == 'N')
    {
      result = false;
    }
  }
  return result;
}

//==============================================================================
bool Parse::BooleanNegative( const char* value )
{
  s_errorCode = ERR_NONE;

  bool  result = false;
  if (value != 0 && value[0] != '\0')
  {
    int dummy;
    if ((sscanf(value, "%d", &dummy) > 0 && dummy > 0) ||
      value[0] == 'y' || value[0] == 'Y')
    {
      result = true;
    }
  }
  return result;
}

//==============================================================================
int   Parse::Int(const char* value)
{
  int v;
  if (!Int(value, v))
  {
    v = 0;
  }
  s_errorCode = ERR_NONE;
  return v;
}

//==============================================================================
int   Parse::Int(int base, const char* value)
{
  int v;
  if (!Int(base, value, v))
  {
    v = 0;
  }
  s_errorCode = ERR_NONE;
  return v;
}

//==============================================================================
bool  Parse::Int( const char* value, int& v)
{
  s_errorCode = ERR_NONE;

  bool  success = value != nullptr;
  if (!success)
  {
    s_errorCode = ERR_NULL_STRING;
  }

  int intValue = 0;
  if (success)
  {
    success = sscanf(value, "%d", &intValue) == 1;
    if (!success)
    {
      s_errorCode = ERR_INVALID_VALUE;
    }
  }

  v = intValue;
  return success;
}

//==============================================================================
bool  Parse::Int(int base, const char* value, int& v)
{
  s_errorCode = ERR_NONE;

  bool  success = value != nullptr;
  if (!success)
  {
    s_errorCode = ERR_NULL_STRING;
  }

  int intValue = 0;
  if (success)
  {
    if (strchr(value, '%') != 0)
    {
      double  percent;
      success = sscanf(value, "%lf", &percent) == 1;
      if (!success)
      {
        s_errorCode = ERR_INVALID_VALUE;
      }

      if (success)
      {
        intValue = static_cast<int>(std::round(float(percent * base / 100.0)));
      }
    }
    else if (strchr(value, 'x') != 0)
    {
      double  multi;
      const char* temp = value;
      if (temp[0] == 'x')
      {
        ++temp;
      }

      success = sscanf(temp, "%lf", &multi) == 1;
      if (!success)
      {
        s_errorCode = ERR_INVALID_VALUE;
      }

      if (success)
      {
        intValue = static_cast<int>(Round(float(multi * base)));
      }
    }
    else if (strchr(value, '.') != 0)
    {
      double  scalar;
      success = sscanf(value, "%lf", &scalar) == 1;
      if (!success)
      {
        s_errorCode = ERR_INVALID_VALUE;
      }

      if (success)
      {
        intValue = static_cast<int>(Round(float(scalar * base)));
      }
    }
    else
    {
      success = sscanf(value, "%d", &intValue) == 1;
      if (!success)
      {
        s_errorCode = ERR_INVALID_VALUE;
      }
    }
  }

  v = intValue;
  return success;
}

//==============================================================================
float Parse::Float(const char* value)
{
  float v;
  if (!Float(value, v))
  {
    v = .0f;
  }
  s_errorCode = ERR_NONE;
  return v;
}

//==============================================================================
bool  Parse::Float(const char* value, float& v)
{
  s_errorCode = ERR_NONE;

  bool  success = value != nullptr;
  if (!success)
  {
    s_errorCode = ERR_NULL_STRING;
  }

  float floatValue = .0f;
  if (success)
  {
    if (strchr(value, '%') != 0)
    {
      double  percent;
      success = sscanf(value, "%lf", &percent) == 1;
      if (!success)
      {
        s_errorCode = ERR_INVALID_VALUE;
      }

      if (success)
      {
        floatValue = float(percent / 100.0);
      }
    }
    else
    {
      success = sscanf(value, "%f", &floatValue) == 1;
      if (!success)
      {
        s_errorCode = ERR_INVALID_VALUE;
      }
    }
  }

  v = floatValue;
  return success;
}

//==============================================================================
bool  Parse::FloatSpeedPerSecond(const char* value, float fps, float& v)
{
  s_errorCode = ERR_NONE;

  XR_ASSERT(Parse::FloatSpeedPerSecond, fps > .0f);
  bool  success = value != nullptr;
  if (!success)
  {
    s_errorCode = ERR_NULL_STRING;
  }

  float floatValue = .0f;
  if (success)
  {
    success = sscanf(value, "%f", &floatValue) == 1;
    if (!success)
    {
      s_errorCode = ERR_INVALID_VALUE;
    }

    if (success)
    {
      floatValue = CalcSpeedPerFrame(floatValue, fps);
    }
  }

  v = floatValue;
  return success;
}

//==============================================================================
bool  Parse::FloatAccelerationPerSecond(const char* value, float fps, float& v)
{
  s_errorCode = ERR_NONE;

  XR_ASSERT(Parse::FloatAccelerationPerSecond, fps > .0f);
  bool  success = value != nullptr;
  if (!success)
  {
    s_errorCode = ERR_NULL_STRING;
  }

  float floatValue = .0f;
  if (success)
  {
    success = sscanf(value, "%f", &floatValue) == 1;
    if (!success)
    {
      s_errorCode = ERR_INVALID_VALUE;
    }

    if (success)
    {
      floatValue = CalcAccelerationPerFrame(floatValue, fps);
    }
  }

  v = floatValue;
  return success;
}

//==============================================================================
bool  Parse::FloatScalingPerSecond(const char* value, float fps, float& v)
{
  s_errorCode = ERR_NONE;

  XR_ASSERT(Parse::FloatScalingPerSecond, fps > .0f);
  bool  success = value != nullptr;
  if (!success)
  {
    s_errorCode = ERR_NULL_STRING;
  }

  float floatValue = .0f;
  if (success)
  {
    success = sscanf(value, "%f", &floatValue) == 1;
    if (!success)
    {
      s_errorCode = ERR_INVALID_VALUE;
    }

    if (success)
    {
      floatValue = CalcScalingPerFrame(floatValue, fps);
    }
  }

  v = floatValue;
  return success;
}

//==============================================================================
bool Parse::Range( const char* value, int& r0, int& r1 )
{
  s_errorCode = ERR_NONE;

  bool  success = value != nullptr;
  if (!success)
  {
    s_errorCode = ERR_NULL_STRING;
  }

  r0 = r1 = 0;
  if (success)
  {
    size_t len = strlen(value);
    success = len > 0 && sscanf(value, "%d", &r0) == 1;
    if (!success)
    {
      s_errorCode = ERR_INVALID_VALUE;
    }

    if (success)
    {
      const char* valueEnd = value + len;
      value = strchr(value, '-');
      if (value != nullptr && r0 < 0)
      {
        value = strchr(value + 1, '-');
      }

      if (value == nullptr)
      {
        r1 = r0;
      }
      else
      {
        ++value;
        success = value != valueEnd && sscanf(value, "%d", &r1) == 1 && r1 >= r0;
      }
    }
  }
  return success;
}

//==============================================================================
bool Parse::Range( const char* value, float& r0, float& r1 )
{
  s_errorCode = ERR_NONE;

  bool  success = value != nullptr;
  if (!success)
  {
    s_errorCode = ERR_NULL_STRING;
  }

  r0 = r1 = 0;
  if (success)
  {
    size_t len = strlen(value);
    success = len > 0 && sscanf(value, "%f", &r0) == 1;
    if (!success)
    {
      s_errorCode = ERR_INVALID_VALUE;
    }

    if (success)
    {
      const char* valueEnd = value + len;
      value = strchr(value, '-');
      if (value != nullptr && r0 < .0f)
      {
        value = strchr(value + 1, '-');
      }

      if (value == nullptr)
      {
        r1 = r0;
      }
      else
      {
        ++value;
        success = value != valueEnd && sscanf(value, "%f", &r1) == 1 && r1 >= r0;
      }
    }
  }
  return success;
}

//==============================================================================
xr::Color Parse::Color( const char* value )
{
  s_errorCode = ERR_NONE;

  xr::Color col(0xffffffff);
  if (value != nullptr)
  {
    uint32_t  rgba;
    if (sscanf(value, "%x", &rgba) > 0)
    {
      if (strlen(value) <= 6)
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

} // xr
