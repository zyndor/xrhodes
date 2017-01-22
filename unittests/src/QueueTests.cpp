#include "stdafx.h"
#include "CppUnitTest.h"
#include <XR/Queue.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XR
{		
	TEST_CLASS(QueueTests)
	{
	public:
		
		TEST_METHOD(Queue_BasicOperations)
		{
      // default construction
      Queue<int>  q;
      Assert::IsTrue(q.empty());
      Assert::IsTrue(q.size() == 0U);

      // push_back()
      for (int i = 0; i < 100; ++i)
      {
        q.push_back(i);
      }

      Assert::IsTrue(!q.empty());
      Assert::IsTrue(q.size() == 100U);

      // remove()
      q.remove(45);
      Assert::IsTrue(q.size() == 99U);

      // traversal and push / remove operations
      auto i = q.begin();
      for (int j = 0; j < 45; ++j)
      {
        Assert::IsTrue(*i == j);
        ++i;
      }

      for (int j = 46; j < 100; ++j)
      {
        Assert::IsTrue(*i == j);
        ++i;
      }

      // front() and pop_front()
      Assert::IsTrue(q.front() == 0);
      q.pop_front();
      Assert::IsTrue(q.size() == 98U);
      Assert::IsTrue(q.front() == 1);

      // copy construction
      auto q2 = q;
      Assert::IsTrue(q.size() == 98U);
      Assert::IsTrue(q2.size() == 98U);
      
      // assignment
      q = q2;
      Assert::IsTrue(q.size() == 98U);
      Assert::IsTrue(q2.size() == 98U);

      i = q.begin();
      for (auto j : q2)
      {
        Assert::IsTrue(j == *i);
        ++i;
      }

      // adopt() -- note that remove() is also implemented based on adopt().
      q.adopt(q2);
      Assert::IsTrue(q.size() == 196U);
      Assert::IsTrue(q2.empty());

      // clear()
      q.clear();
      Assert::IsTrue(q.empty());
      Assert::IsTrue(q.size() == 0U);
    }

	};
}