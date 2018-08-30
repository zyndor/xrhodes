#ifndef XR_GAMECONTROLLER_HPP
#define XR_GAMECONTROLLER_HPP
//==============================================================================
//
// XRhodes
//
// copyright (c) Nuclear Heart Interactive Ltd. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xr/Vector2.hpp"
#include "xr/ButtonState.hpp"
#include "xr/HatState.hpp"
#include "SDL_joystick.h"
#include <vector>
#include <cstdint>

namespace xr
{

class GameController
{
public:
  static float Normalize(int32_t value)
  {
    return (value + .5f) / ((1 << 15) - .5f);
  }

  ~GameController()
  {
    if (IsActive())
    {
      Close();
    }
  }

  uint32_t Open(uint32_t id)
  {
    XR_ASSERT(GameController, m_handle == nullptr);
    m_handle = SDL_JoystickOpen(id);

    m_axes.resize(SDL_JoystickNumAxes(m_handle));
    m_buttons.resize(SDL_JoystickNumButtons(m_handle));
    m_hats.resize(SDL_JoystickNumHats(m_handle));
    m_balls.resize(SDL_JoystickNumBalls(m_handle));

    // Return the instance ID that all subsequent events will refer to.
    return static_cast<uint32_t>(SDL_JoystickInstanceID(m_handle));
  }

  bool IsActive() const
  {
    return m_handle != nullptr;
  }

  char const* GetName() const
  {
    return IsActive() ? SDL_JoystickName(m_handle) : nullptr;
  }

  void Update()
  {
    if (IsActive())
    {
      for (auto& b: m_buttons)
      {
        ButtonState::Poll(b);
      }

      for (auto& h: m_hats)
      {
        h = h >> HatState::kShift;
      }
    }
  }

  uint32_t GetNumAxes() const
  {
    return static_cast<uint32_t>(m_axes.size());
  }

  uint32_t GetNumButtons() const
  {
    return static_cast<uint32_t>(m_buttons.size());
  }

  uint32_t GetNumHats() const
  {
    return static_cast<uint32_t>(m_hats.size());
  }

  uint32_t GetNumBalls() const
  {
    return static_cast<uint32_t>(m_balls.size());
  }

  float& GetAxis(uint32_t axis)
  {
    XR_ASSERT(GameController, IsActive() && axis < m_axes.size());
    return m_axes[axis];
  }

  ButtonState::Type& GetButton(uint32_t button)
  {
    XR_ASSERT(GameController, IsActive() && button < m_buttons.size());
    return m_buttons[button];
  }

  HatState::Type& GetHat(uint32_t hat)
  {
    XR_ASSERT(GameController, IsActive() && hat < m_hats.size());
    return m_hats[hat];
  }

  Vector2& GetBall(uint32_t ball)
  {
    XR_ASSERT(GameController, IsActive() && ball < m_balls.size());
    return m_balls[ball];
  }

  void Close()
  {
    SDL_JoystickClose(m_handle);
    m_handle = nullptr;

    m_buttons.resize(0);
    m_axes.resize(0);
    m_hats.resize(0);
    m_balls.resize(0);
  }

private:
  SDL_Joystick* m_handle = nullptr;

  std::vector<float> m_axes;
  std::vector<ButtonState::Type> m_buttons;
  std::vector<HatState::Type> m_hats;
  std::vector<Vector2> m_balls;
};

}

#endif //XR_GAMECONTROLLER_HPP
