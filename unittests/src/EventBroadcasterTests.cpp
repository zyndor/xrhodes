#include "stdafx.h"
#include "CppUnitTest.h"
#include <XR/EventBroadcaster.hpp>
#include <memory>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XR
{
  TEST_CLASS(EventBroadcasterTests)
  {
  public:

    struct Int
    {
      int value = 0;

      void OnAdd(int i)
      {
        value += i;
      }

      void OnMultiply(int i)
      {
        value *= i;
      }

      operator int() const
      {
        return value;
      }
    };

    TEST_METHOD(EventBroadcaster_Basics)
    {
      EventBroadcaster<int> onAdded;
      EventBroadcaster<int> onMultiplied;

      Int a;
      Assert::IsTrue(onAdded.AddListener(a, &Int::OnAdd)); // successful add
      Assert::IsFalse(onAdded.AddListener(a, &Int::OnAdd));  // already added - unsuccessful
      onMultiplied.AddListener(a, &Int::OnMultiply);

      Int b;
      b.value = 2;
      onAdded.AddListener(b, &Int::OnAdd);

      onAdded.Broadcast(10);
      Assert::IsTrue(a == 10);
      Assert::IsTrue(b == 12);

      onMultiplied.Broadcast(2);
      Assert::IsTrue(a == 20);
      Assert::IsTrue(b == 12);

      onAdded.RemoveListener(&a);
      onAdded.Broadcast(4);
      Assert::IsTrue(a == 20);
      Assert::IsTrue(b == 16);

      onMultiplied.Clear();
      onMultiplied.Broadcast(5);
      Assert::IsTrue(a == 20);
      Assert::IsTrue(b == 16);
    }

    struct EventCounter
    {
      int eventsReceived = 0;

      void OnEvent()
      {
        ++eventsReceived;
      }
    };

    struct EventCounterHolder: EventCounter
    {
      EventBroadcaster<>* pBroadcaster;
      EventCounter counter;
      std::unique_ptr<bool> result;

      void SetBroadcaster(EventBroadcaster<>& bc)
      {
        pBroadcaster = &bc;
      }

      void OnAdd()
      {
        bool r = pBroadcaster->AddListener(counter, &EventCounter::OnEvent);
        result.reset(new bool(r));
        OnEvent();
      }

      void OnRemove()
      {
        bool r = pBroadcaster->RemoveListener(&counter);
        result.reset(new bool(r));
        OnEvent();
      }

      void OnClear()
      {
        pBroadcaster->Clear();
        OnEvent();
      }
    };

    TEST_METHOD(EventBroadcaster_PostponedAdd)
    {
      EventBroadcaster<> onEvent;
      EventCounterHolder addTester;
      addTester.SetBroadcaster(onEvent);

      onEvent.AddListener(addTester, &EventCounterHolder::OnAdd);

      onEvent.Broadcast();
      Assert::IsTrue(addTester.eventsReceived == 1);  // did receive event
      Assert::IsTrue(*addTester.result);  // succesful add
      Assert::IsTrue(addTester.counter.eventsReceived == 0);  // did not receive event - added postponed

      onEvent.Broadcast();
      Assert::IsTrue(addTester.eventsReceived == 2);  // did receive event
      Assert::IsTrue(!*addTester.result);  // unsuccessful add - alreday added
      Assert::IsTrue(addTester.counter.eventsReceived == 1);  // did receive event
    }

    TEST_METHOD(EventBroadcaster_PostponedRemove)
    {
      EventBroadcaster<> onEvent;
      EventCounterHolder removeTester;
      removeTester.SetBroadcaster(onEvent);

      onEvent.AddListener(removeTester, &EventCounterHolder::OnRemove);
      onEvent.AddListener(removeTester.counter, &EventCounter::OnEvent);

      onEvent.Broadcast();
      Assert::IsTrue(removeTester.eventsReceived == 1);  // did receive event
      Assert::IsTrue(*removeTester.result);  // succesful remove
      Assert::IsTrue(removeTester.counter.eventsReceived == 1);  // did receive event - removed postponed

      onEvent.Broadcast();
      Assert::IsTrue(removeTester.eventsReceived == 2);  // did receive event
      Assert::IsTrue(!*removeTester.result);  // unsuccessful remove - alreday removed
      Assert::IsTrue(removeTester.counter.eventsReceived == 1);  // did not receive event
    }

    TEST_METHOD(EventBroadcaster_PostponedClear)
    {
      EventBroadcaster<> onEvent;
      EventCounterHolder clearTester;
      clearTester.SetBroadcaster(onEvent);

      onEvent.AddListener(clearTester, &EventCounterHolder::OnClear);
      onEvent.AddListener(clearTester.counter, &EventCounter::OnEvent);

      onEvent.Broadcast();
      Assert::IsTrue(clearTester.eventsReceived == 1);  // did receive event
      Assert::IsTrue(clearTester.counter.eventsReceived == 1);  // did receive event - removed postponed

      onEvent.Broadcast();
      Assert::IsTrue(clearTester.eventsReceived == 1);  // did not receive event
      Assert::IsTrue(clearTester.counter.eventsReceived == 1);  // did not receive event
    }
  };
}
