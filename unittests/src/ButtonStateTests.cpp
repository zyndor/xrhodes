//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "gtest/gtest.h"
#include "xr/ButtonState.hpp"
#include "xr/utils.hpp"

using namespace xr;

namespace
{

TEST(ButtonState, Basics)
{
  ButtonState::Type button = ButtonState::Up;

  ButtonState::SetAbsolute(true, button); // button down event
  ASSERT_EQ(button, ButtonState::kAbsoluteStateBit);

  ButtonState::Poll(button);  // new frame - button is now pressed.
  ASSERT_TRUE(CheckAllMaskBits(button, ButtonState::IsPressed));
  ASSERT_TRUE(!CheckAllMaskBits(button, ButtonState::WasPressed));
  ASSERT_TRUE(CheckAllMaskBits(button, ButtonState::kAbsoluteStateBit));

  ButtonState::Poll(button);  // new frame - button is now held.
  ASSERT_TRUE(CheckAllMaskBits(button, ButtonState::Down));
  ASSERT_TRUE(CheckAllMaskBits(button, ButtonState::kAbsoluteStateBit));

  ButtonState::Poll(button);  // new frame - button is still held.
  ASSERT_TRUE(CheckAllMaskBits(button, ButtonState::Down));
  ASSERT_TRUE(CheckAllMaskBits(button, ButtonState::kAbsoluteStateBit));

  ButtonState::SetAbsolute(false, button);  // button release event
  ASSERT_TRUE(!CheckAllMaskBits(button, ButtonState::kAbsoluteStateBit));
  ASSERT_TRUE(CheckAllMaskBits(button, ButtonState::Down));

  ButtonState::Poll(button);  // new frame - button was pressed
  ASSERT_TRUE(CheckAllMaskBits(button, ButtonState::WasPressed));
  ASSERT_TRUE(!CheckAllMaskBits(button, ButtonState::IsPressed));
  ASSERT_TRUE(!CheckAllMaskBits(button, ButtonState::kAbsoluteStateBit));

  ButtonState::Poll(button);  // new frame - button is up.
  ASSERT_TRUE(!CheckAllMaskBits(button, ButtonState::WasPressed));
  ASSERT_TRUE(!CheckAllMaskBits(button, ButtonState::IsPressed));
  ASSERT_TRUE(!CheckAllMaskBits(button, ButtonState::kAbsoluteStateBit));
}

}
