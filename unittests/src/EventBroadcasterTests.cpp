#include "gtest/gtest.h"
#include "xr/EventBroadcaster.hpp"
#include <memory>

namespace xr
{
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

  TEST(EventBroadcaster, Basics)
  {
    EventBroadcaster<int> onAdded;
    EventBroadcaster<int> onMultiplied;

    Int a;
    ASSERT_TRUE(onAdded.AddListener(a, &Int::OnAdd)); // successful add
    ASSERT_FALSE(onAdded.AddListener(a, &Int::OnAdd));  // already added - unsuccessful
    onMultiplied.AddListener(a, &Int::OnMultiply);

    Int b;
    b.value = 2;
    onAdded.AddListener(b, &Int::OnAdd);

    onAdded.Broadcast(10);
    ASSERT_EQ(a, 10);
    ASSERT_EQ(b, 12);

    onMultiplied.Broadcast(2);
    ASSERT_EQ(a, 20);
    ASSERT_EQ(b, 12);

    onAdded.RemoveListener(&a);
    onAdded.Broadcast(4);
    ASSERT_EQ(a, 20);
    ASSERT_EQ(b, 16);

    onMultiplied.Clear();
    onMultiplied.Broadcast(5);
    ASSERT_EQ(a, 20);
    ASSERT_EQ(b, 16);
  }

  struct EventCounter
  {
    int eventsReceived = 0;

    void OnEvent()
    {
      ++eventsReceived;
    }
  };

  struct EventCounterHolder : EventCounter
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

  TEST(EventBroadcaster, PostponedAdd)
  {
    EventBroadcaster<> onEvent;
    EventCounterHolder addTester;
    addTester.SetBroadcaster(onEvent);

    onEvent.AddListener(addTester, &EventCounterHolder::OnAdd);

    onEvent.Broadcast();
    ASSERT_EQ(addTester.eventsReceived, 1);  // did receive event
    ASSERT_TRUE(*addTester.result);  // succesful add
    ASSERT_EQ(addTester.counter.eventsReceived, 0);  // did not receive event - added postponed

    onEvent.Broadcast();
    ASSERT_EQ(addTester.eventsReceived, 2);  // did receive event
    ASSERT_TRUE(!*addTester.result);  // unsuccessful add - alreday added
    ASSERT_EQ(addTester.counter.eventsReceived, 1);  // did receive event
  }

  TEST(EventBroadcaster, PostponedRemove)
  {
    EventBroadcaster<> onEvent;
    EventCounterHolder removeTester;
    removeTester.SetBroadcaster(onEvent);

    onEvent.AddListener(removeTester, &EventCounterHolder::OnRemove);
    onEvent.AddListener(removeTester.counter, &EventCounter::OnEvent);

    onEvent.Broadcast();
    ASSERT_EQ(removeTester.eventsReceived, 1);  // did receive event
    ASSERT_TRUE(*removeTester.result);  // succesful remove
    ASSERT_EQ(removeTester.counter.eventsReceived, 1);  // did receive event - removed postponed

    onEvent.Broadcast();
    ASSERT_EQ(removeTester.eventsReceived, 2);  // did receive event
    ASSERT_TRUE(!*removeTester.result);  // unsuccessful remove - alreday removed
    ASSERT_EQ(removeTester.counter.eventsReceived, 1);  // did not receive event
  }

  TEST(EventBroadcaster, PostponedClear)
  {
    EventBroadcaster<> onEvent;
    EventCounterHolder clearTester;
    clearTester.SetBroadcaster(onEvent);

    onEvent.AddListener(clearTester, &EventCounterHolder::OnClear);
    onEvent.AddListener(clearTester.counter, &EventCounter::OnEvent);

    onEvent.Broadcast();
    ASSERT_EQ(clearTester.eventsReceived, 1);  // did receive event
    ASSERT_EQ(clearTester.counter.eventsReceived, 1);  // did receive event - removed postponed

    onEvent.Broadcast();
    ASSERT_EQ(clearTester.eventsReceived, 1);  // did not receive event
    ASSERT_EQ(clearTester.counter.eventsReceived, 1);  // did not receive event
  }
}
