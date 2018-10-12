//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "gtest/gtest.h"
#include "xr/events/SignalNotifier.hpp"
#include "xr/types/fundamentals.hpp"
#include <list>

using namespace xr;

namespace
{

bool OnEvent(void* userData)
{
  *static_cast<bool*>(userData) = true;
  return false;
}

struct Tester : InstCounted<Tester>
{
  int const id;
  bool handled = false;
  bool notified = false;

  Tester()
  : id(GetInstanceCount())
  {}

  bool OnEvent()
  {
    handled = id == 2;
    notified = true;
    return handled;
  }
};
}

TEST(SignalNotifier, Basics)
{
  std::list<Tester> testers;

  testers.push_back(Tester());
  testers.push_back(Tester());
  testers.push_back(Tester());

  SignalNotifier<> notifier;

  // Successful registration.
  bool notified = false;
  ASSERT_TRUE(notifier.Connect(FunctionPtrCallback<bool>(OnEvent, &notified)));

  for (auto& t : testers)
  {
    ASSERT_TRUE(notifier.Connect(MakeCallback(t, &Tester::OnEvent)));
  }

  // Unsuccessful registration -- already registered.
  ASSERT_FALSE(notifier.Connect(FunctionPtrCallback<bool>(OnEvent, &notified)));

  for (auto& t : testers)
  {
    ASSERT_FALSE(notifier.Connect(MakeCallback(t, &Tester::OnEvent)));
  }

  // Check notification. All testers should be notified up to and including #2.
  // #2 should only handle the notification.
  ASSERT_TRUE(notifier.Notify());
  ASSERT_TRUE(notified);

  for (auto const& t : testers)
  {
    ASSERT_EQ(t.handled, (t.id == 2));
    ASSERT_EQ(t.notified, (t.id <= 2));
  }

  // Successful removal.
  ASSERT_TRUE(notifier.Disconnect(FunctionPtrCallback<bool>(OnEvent, &notified)));

  for (auto& t : testers)
  {
    ASSERT_TRUE(notifier.Disconnect(MakeCallback(t, &Tester::OnEvent)));
  }

  // Unsuccessful removal -- already removed.
  ASSERT_FALSE(notifier.Disconnect(FunctionPtrCallback<bool>(OnEvent, &notified)));

  for (auto& t : testers)
  {
    ASSERT_FALSE(notifier.Disconnect(MakeCallback(t, &Tester::OnEvent)));
  }
}
