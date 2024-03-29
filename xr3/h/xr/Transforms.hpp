#ifndef XR_TRANSFORMS_HPP
#define XR_TRANSFORMS_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/types/fundamentals.hpp"

namespace xr
{

class Matrix;
class Matrix4;
class ViewRayCaster;

//==============================================================================
///@brief Enables the management of model-view-projection transformations,
/// offering a stack for model matrices and a single view and projection matrix,
/// whose value updates the following set of built in uniforms:
/// xruModel                mat4
/// xruModelView            mat4
/// xruView                 mat4
/// xruViewProjection       mat4
/// xruProjection           mat4
/// xruModelViewProjection  mat4
/// xruNormal               mat3 - normal matrix (inverse transpose of xruModelView).
/// Upon initialisation, model and view are set to identity, and a perspective
/// projection is set up.
class Transforms
{
  XR_NONOBJECT_DECL(Transforms)

public:
  // types
  enum
  {
    kNumMatrixElems = 16
  };

  ///@brief Provides facilities for concatenating updates to the model, view
  /// and / or projection matrices. It will issue the update of the uniforms
  /// upon going out of scope, at once.
  ///@note Ideally only created on stack within a function scope.
  ///@note Starting an update when one is already in progress will likely lead
  /// to unexpected and incorrect results.
  class Updater
  {
  public:
    Updater();
    ~Updater();

    ///@brief Clears the model stack and pushes the given matrix @a m.
    Updater& SetModel(Matrix const& m);

    ///@brief Transforms the model matrix by @a m.
    Updater& PushModel(Matrix const& m);

    ///@brief Reverts the stack to the previous matrix, if any.
    ///@note The model stack must always have at least one entry, meaning
    /// that all PopModel() calls must correspond to a previous PushModel().
    Updater& PopModel();

    ///@brief Calculates the view matrix from viewer position and orientation.
    Updater& SetViewerTransform(Matrix const& m);

    ///@brief Sets the view to the given @a matrix.
    Updater& SetView(Matrix4 const& matrix);

    ///@brief Sets the projection to the given @a matrix. Transforms caches
    /// zNear, zFar and tanHalfVerticalFov values for internal operations.
    /// If they were not provided (i.e. are zero), they will be calculated
    /// from @a matrix as follows:<br />
    /// tanHalfVerticalFov = 1.f / matrix[5];
    /// zNear = matrix[14] / (matrix[10] - 1.f);
    /// zFar = (matrix[10] - 1.f) / (matrix[10] + 1.f) * zNear;
    Updater& SetProjection(Matrix4 const& matrix, float zNear = .0f, float zFar = .0f,
      float tanHalfVerticalFov = .0f);

    ///@brief Sets up orthographic projection.
    ///@param zNear distance of the near viewing plane.
    ///@param zFar distance of the far viewing plane.
    Updater& SetOrthographicProjection(float left, float right, float bottom,
      float top, float zNear, float zFar);

    ///@brief Sets up perspective projection.
    ///@param verticalFov Vertical field of view in radians.
    ///@param aspectRatio The ratio of the projection width to (/ over) height.
    ///@param zNear The distance of the near clipping pane from the camera.
    ///@param zFar The distance of the far clipping pane from the camera.
    Updater& SetPerspectiveProjection(float verticalFov, float aspectRatio,
      float zNear, float zFar);

    ///@brief Sets up perspective projection. The aspect ratio is calculated from the
    /// current screen width and height known to Gfx.
    Updater& SetPerspectiveProjection(float verticalFov, float zNear, float zFar);
  };

  ///@brief Pushes the given model matrix at construction, then pops it off
  /// upon destruction.
  class ScopedModel
  {
    XR_NONCOPY_DECL(ScopedModel)

  public:
    explicit ScopedModel(Matrix const& m)
    {
      Updater().PushModel(m);
    }

    ~ScopedModel()
    {
      Updater().PopModel();
    }
  };

  // static
  ///@brief Initialises Transforms with an identity model and view and a
  /// perspective projection.
  ///@note Gfx::Init() must be called beforehand.
  static void Init();

  ///@brief Copies the current model matrix, which is the product of all model
  /// matrices pushed to the stack.
  static void GetModel(Matrix& m);

  ///@brief Copies the matrix defining the position and orientation of the
  /// viewer, to @a m.
  ///@note This is the inverse of the view matrix.
  static void GetViewerTransform(Matrix& m);

  ///@brief Copies the view matrix into @a matrix.
  static void GetView(Matrix4& matrix);

  ///@brief Copies the projection matrix into @a matrix.
  static void GetProjection(Matrix4& matrix);

  ///@return The distance of the near clipping plane from the viewer.
  static float GetZNearClippingPlane();

  ///@return The distance of the far clipping plane from the viewer.
  static float GetZFarClippingPlane();

  ///@return The perspective multiple. Multiply this by the projection height to
  /// get the (view space) z position at which objects appear at a scale of 1.
  ///@note As the name implies, it's only meaningful using a perspective projection.
  static float GetPerspectiveMultiple();

  ///@brief Creates a ViewRayCaster with the current view and projection settings,
  /// and the given @a aspectRatio.
  static ViewRayCaster GetViewRayCaster(float aspectRatio);

  [[deprecated("Logical aspect ratio of the entire screen is used; provide aspect ratio of your viewport.")]]
  static ViewRayCaster GetViewRayCaster();
};

} // xr

//==============================================================================
#define XR_TRANSFORMS_SCOPED_MODEL_NAME_(l) xrScopedModel ## l
#define XR_TRANSFORMS_SCOPED_MODEL_NAME(l) XR_TRANSFORMS_SCOPED_MODEL_NAME_(l)

///@brief Facilitates the usage of Transforms::ScopedModel without having to
/// come up with a new name for the object instance (as long as it isn't used
/// more than once per line).
#define XR_TRANSFORMS_SCOPED_MODEL(m) xr::Transforms::ScopedModel XR_TRANSFORMS_SCOPED_MODEL_NAME(__LINE__)(m);

#endif // XR_TRANSFORMS_HPP
