//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "DragController.hpp"

using namespace xr;

namespace
{

constexpr double kMaxPressDurationMs = 2000.;

}

void DragController::Reset()
{
  m_tLastFrameMs = Timer::GetUST();
  m_pressTimerMs = 0.;
  for (auto& i : m_accumulators)
  {
    i = Vector2{};
  }
}

bool DragController::ConnectMouse()
{
  bool result = Input::MouseActionSignal().
    Connect(MakeCallback(*this, &DragController::OnMouseAction));
  result = Input::MouseMotionSignal().
    Connect(MakeCallback(*this, &DragController::OnMouseMotion)) || result;
  return result;
}

bool DragController::DisconnectMouse()
{
  bool result = Input::MouseActionSignal().
    Disconnect(MakeCallback(*this, &DragController::OnMouseAction));
  result = Input::MouseMotionSignal().
    Disconnect(MakeCallback(*this, &DragController::OnMouseMotion)) || result;
  return result;
}

void DragController::Update()
{
  auto now = Timer::GetUST();
  auto timer = m_pressTimerMs + BoolToSign<double>(m_isButtonDown) *
    (now - m_tLastFrameMs);
  m_pressTimerMs = std::min(std::max(0., timer), kMaxPressDurationMs);
  m_tLastFrameMs = now;

  m_motion += m_accumulators[m_iActiveAccumulator];
  int i = NextAccumulator();
  m_iActiveAccumulator = i;
  m_accumulators[i] = Vector2::Zero();
}

float DragController::GetPressTimerPercent() const
{
  return static_cast<float>(m_pressTimerMs / kMaxPressDurationMs);
}

xr::Vector2 const& DragController::GetFrameMotion() const
{
  return m_accumulators[NextAccumulator()];
}

xr::Vector2 const & DragController::GetTotalMotion() const
{
  return m_motion;
}

int DragController::NextAccumulator() const
{
  return (m_iActiveAccumulator + 1) & 1;
}

void DragController::OnMouseAction(xr::Input::MouseActionData const& e)
{
  if (e.button == MouseButton::Left)
  {
    m_isButtonDown = e.isPressed;
    m_lastMousePosition.x = e.x;
    m_lastMousePosition.y = e.y;
  }
}

void DragController::OnMouseMotion(xr::Input::MouseMotionData const& e)
{
  if (m_isButtonDown)
  {
    m_accumulators[m_iActiveAccumulator] +=
      Vector2(float(e.x - m_lastMousePosition.x), float(e.y - m_lastMousePosition.y));
    m_lastMousePosition.x = e.x;
    m_lastMousePosition.y = e.y;
  }
}
