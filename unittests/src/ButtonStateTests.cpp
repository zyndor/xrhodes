//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xm.hpp"
#include "xr/ButtonState.hpp"
#include "xr/utils.hpp"

using namespace xr;

namespace
{

XM_TEST(ButtonState, Basics)
{
  ButtonState::Type button = ButtonState::Up;

  ButtonState::SetAbsolute(true, button); // button down event
  XM_ASSERT_EQ(button, ButtonState::kAbsoluteStateBit);

  ButtonState::Poll(button);  // new frame - button is now pressed.
  XM_ASSERT_TRUE(CheckAllMaskBits(button, ButtonState::IsPressed));
  XM_ASSERT_TRUE(!CheckAllMaskBits(button, ButtonState::WasPressed));
  XM_ASSERT_TRUE(CheckAllMaskBits(button, ButtonState::kAbsoluteStateBit));

  ButtonState::Poll(button);  // new frame - button is now held.
  XM_ASSERT_TRUE(CheckAllMaskBits(button, ButtonState::Down));
  XM_ASSERT_TRUE(CheckAllMaskBits(button, ButtonState::kAbsoluteStateBit));

  ButtonState::Poll(button);  // new frame - button is still held.
  XM_ASSERT_TRUE(CheckAllMaskBits(button, ButtonState::Down));
  XM_ASSERT_TRUE(CheckAllMaskBits(button, ButtonState::kAbsoluteStateBit));

  ButtonState::SetAbsolute(false, button);  // button release event
  XM_ASSERT_TRUE(!CheckAllMaskBits(button, ButtonState::kAbsoluteStateBit));
  XM_ASSERT_TRUE(CheckAllMaskBits(button, ButtonState::Down));

  ButtonState::Poll(button);  // new frame - button was pressed
  XM_ASSERT_TRUE(CheckAllMaskBits(button, ButtonState::WasPressed));
  XM_ASSERT_TRUE(!CheckAllMaskBits(button, ButtonState::IsPressed));
  XM_ASSERT_TRUE(!CheckAllMaskBits(button, ButtonState::kAbsoluteStateBit));

  ButtonState::Poll(button);  // new frame - button is up.
  XM_ASSERT_TRUE(!CheckAllMaskBits(button, ButtonState::WasPressed));
  XM_ASSERT_TRUE(!CheckAllMaskBits(button, ButtonState::IsPressed));
  XM_ASSERT_TRUE(!CheckAllMaskBits(button, ButtonState::kAbsoluteStateBit));
}

}
