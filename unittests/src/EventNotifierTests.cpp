#include <gtest/gtest.h>
#include "XR/EventNotifier.hpp"
#include "XR/fundamentals.hpp"

namespace XR
{
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

  TEST(EventNotifier, Basics)
  {
    std::list<Tester> testers;

    testers.push_back(Tester());
    testers.push_back(Tester());
    testers.push_back(Tester());

    EventNotifier<> notifier;

    // Successful registration.
    for (auto& t : testers)
    {
      ASSERT_TRUE(notifier.AddListener(t, &Tester::OnEvent));
    }

    // Unsuccessful registration -- already registered.
    for (auto& t : testers)
    {
      ASSERT_FALSE(notifier.AddListener(t, &Tester::OnEvent));
    }

    // Check notification. All testers should be notified up to and including #2.
    // #2 should only handle the notification.
    ASSERT_TRUE(notifier.Notify());
    for (auto const& t : testers)
    {
      ASSERT_EQ(t.handled, (t.id == 2));
      ASSERT_EQ(t.notified, (t.id <= 2));
    }

    // Successful removal.
    for (auto& t : testers)
    {
      ASSERT_TRUE(notifier.RemoveListener(&t));
    }

    // Unsuccessful removal -- already removed.
    for (auto& t : testers)
    {
      ASSERT_FALSE(notifier.RemoveListener(&t));
    }
  }
}
