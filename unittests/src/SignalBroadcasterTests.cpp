//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xm.hpp"
#include "xr/events/SignalBroadcaster.hpp"
#include <memory>

using namespace xr;

namespace
{

void OnAdd(int v, void* userData)
{
  *static_cast<int*>(userData) += v;
}

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
  SignalBroadcaster<>* pBroadcaster;
  EventCounter counter;
  std::unique_ptr<bool> result;

  void SetBroadcaster(SignalBroadcaster<>& bc)
  {
    pBroadcaster = &bc;
  }

  void OnAdd()
  {
    bool r = pBroadcaster->Connect(MakeCallback(counter, &EventCounter::OnEvent));
    result.reset(new bool(r));
    OnEvent();
  }

  void OnRemove()
  {
    bool r = pBroadcaster->Disconnect(MakeCallback(counter, &EventCounter::OnEvent));
    result.reset(new bool(r));
    OnEvent();
  }
};

XM_TEST(SignalBroadcaster, Basics)
{
  SignalBroadcaster<int> onAdded;
  SignalBroadcaster<int> onMultiplied;

  Int a;
  XM_ASSERT_TRUE(onAdded.Connect(MakeCallback(a, &Int::OnAdd))); // successful add
  XM_ASSERT_FALSE(onAdded.Connect(MakeCallback(a, &Int::OnAdd)));  // already added - unsuccessful
  onMultiplied.Connect(MakeCallback(a, &Int::OnMultiply));

  Int b;
  b.value = 2;
  onAdded.Connect(MakeCallback(b, &Int::OnAdd));

  Int c;
  c.value = 15;
  onAdded.Connect(FunctionPtrCallback<void, int>(OnAdd, &c.value));

  onAdded.Broadcast(10);
  XM_ASSERT_EQ(a, 10);
  XM_ASSERT_EQ(b, 12);

  onMultiplied.Broadcast(2);
  XM_ASSERT_EQ(a, 20);
  XM_ASSERT_EQ(b, 12);

  onAdded.Disconnect(MakeCallback(a, &Int::OnAdd));
  onAdded.Broadcast(4);
  XM_ASSERT_EQ(a, 20);
  XM_ASSERT_EQ(b, 16);
}

XM_TEST(SignalBroadcaster, PostponedAdd)
{
  SignalBroadcaster<> onEvent;
  EventCounterHolder addTester;
  addTester.SetBroadcaster(onEvent);

  onEvent.Connect(MakeCallback(addTester, &EventCounterHolder::OnAdd));

  onEvent.Broadcast();
  XM_ASSERT_EQ(addTester.eventsReceived, 1);  // did receive event
  XM_ASSERT_TRUE(*addTester.result);  // succesful add
  XM_ASSERT_EQ(addTester.counter.eventsReceived, 0);  // did not receive event - added postponed

  onEvent.Broadcast();
  XM_ASSERT_EQ(addTester.eventsReceived, 2);  // did receive event
  XM_ASSERT_TRUE(!*addTester.result);  // unsuccessful add - alreday added
  XM_ASSERT_EQ(addTester.counter.eventsReceived, 1);  // did receive event
}

XM_TEST(SignalBroadcaster, PostponedRemove)
{
  SignalBroadcaster<> onEvent;
  EventCounterHolder removeTester;
  removeTester.SetBroadcaster(onEvent);

  onEvent.Connect(MakeCallback(removeTester, &EventCounterHolder::OnRemove));
  onEvent.Connect(MakeCallback(removeTester.counter, &EventCounter::OnEvent));

  onEvent.Broadcast();
  XM_ASSERT_EQ(removeTester.eventsReceived, 1);  // did receive event
  XM_ASSERT_TRUE(*removeTester.result);  // succesful remove
  XM_ASSERT_EQ(removeTester.counter.eventsReceived, 1);  // did receive event - removed postponed

  onEvent.Broadcast();
  XM_ASSERT_EQ(removeTester.eventsReceived, 2);  // did receive event
  XM_ASSERT_TRUE(!*removeTester.result);  // unsuccessful remove - alreday removed
  XM_ASSERT_EQ(removeTester.counter.eventsReceived, 1);  // did not receive event
}

}
