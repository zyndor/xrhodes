//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xm.hpp"
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

XM_TEST(SignalNotifier, Basics)
{
  std::list<Tester> testers;

  testers.push_back(Tester());
  testers.push_back(Tester());
  testers.push_back(Tester());

  SignalNotifier<> notifier;

  // Successful registration.
  bool notified = false;
  XM_ASSERT_TRUE(notifier.Connect(FunctionPtrCallback<bool>(OnEvent, &notified)));

  for (auto& t : testers)
  {
    XM_ASSERT_TRUE(notifier.Connect(MakeCallback(t, &Tester::OnEvent)));
  }

  // Unsuccessful registration -- already registered.
  XM_ASSERT_FALSE(notifier.Connect(FunctionPtrCallback<bool>(OnEvent, &notified)));

  for (auto& t : testers)
  {
    XM_ASSERT_FALSE(notifier.Connect(MakeCallback(t, &Tester::OnEvent)));
  }

  // Check notification. All testers should be notified up to and including #2.
  // #2 should only handle the notification.
  XM_ASSERT_TRUE(notifier.Notify());
  XM_ASSERT_TRUE(notified);

  for (auto const& t : testers)
  {
    XM_ASSERT_EQ(t.handled, (t.id == 2));
    XM_ASSERT_EQ(t.notified, (t.id <= 2));
  }

  // Successful removal.
  XM_ASSERT_TRUE(notifier.Disconnect(FunctionPtrCallback<bool>(OnEvent, &notified)));

  for (auto& t : testers)
  {
    XM_ASSERT_TRUE(notifier.Disconnect(MakeCallback(t, &Tester::OnEvent)));
  }

  // Unsuccessful removal -- already removed.
  XM_ASSERT_FALSE(notifier.Disconnect(FunctionPtrCallback<bool>(OnEvent, &notified)));

  for (auto& t : testers)
  {
    XM_ASSERT_FALSE(notifier.Disconnect(MakeCallback(t, &Tester::OnEvent)));
  }
}

}
