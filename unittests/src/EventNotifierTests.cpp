#include "stdafx.h"
#include "CppUnitTest.h"
#include <XR/EventNotifier.hpp>
#include <XR/fundamentals.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XR
{
  TEST_CLASS(EventNotifierTests)
  {
  public:
    struct Tester: InstCounted<Tester>
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

    TEST_METHOD(EventNotifier_Basics)
    {
      std::list<Tester> testers;

      testers.push_back(Tester());
      testers.push_back(Tester());
      testers.push_back(Tester());

      EventNotifier<> notifier;

      // Successful registration.
      for (auto& t : testers)
      {
        Assert::IsTrue(notifier.AddListener(t, &Tester::OnEvent));
      }

      // Unsuccessful registration -- already registered.
      for (auto& t : testers)
      {
        Assert::IsFalse(notifier.AddListener(t, &Tester::OnEvent));
      }

      // Check notification. All testers should be notified up to and including #2.
      // #2 should only handle the notification.
      Assert::IsTrue(notifier.Notify());
      for (auto const& t : testers)
      {
        Assert::IsTrue(t.handled == (t.id == 2));
        Assert::IsTrue(t.notified == (t.id <= 2));
      }

      // Successful removal.
      for (auto& t : testers)
      {
        Assert::IsTrue(notifier.RemoveListener(&t));
      }

      // Unsuccessful removal -- already removed.
      for (auto& t : testers)
      {
        Assert::IsFalse(notifier.RemoveListener(&t));
      }
    }
  };
}