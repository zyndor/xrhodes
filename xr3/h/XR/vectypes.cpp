#include "vectypes.hpp"

namespace XR
{

//==============================================================================
SVector2 SVector2::s_zero(0, 0);

Vector2 Vector2::s_zero(.0f, .0f);

//==============================================================================
Vector3 Vector3::s_zero(.0f, .0f, .0f);
Vector3 Vector3::s_one(1.0f, 1.0f, 1.0f);
Vector3 Vector3::s_xAxis(1.0f, .0f, .0f);
Vector3 Vector3::s_yAxis(.0f, 1.0f, .0f);
Vector3 Vector3::s_zAxis(.0f, .0f, 1.0f);

//==============================================================================
const float Matrix::karIdentityData[] =
{
  1.0f, .0f, .0f,
  .0f, 1.0f, .0f,
  .0f, .0f, 1.0f
};

Matrix Matrix::s_identity(karIdentityData);

//==============================================================================
const float Quaternion::karUnitQuaternionData[] =
{
  1.0f,
  .0f,
  .0f,
  .0f
};

Quaternion Quaternion::s_unit(karUnitQuaternionData);

} // XR