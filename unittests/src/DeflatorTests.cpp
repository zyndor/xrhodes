#include "stdafx.h"
#include "CppUnitTest.h"
#include <XR/Deflator.hpp>
#include <XR/streamutils.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XR
{
  TEST_CLASS(DeflatorTests)
  {
  public:

    struct A : public XR::Inflatable
    {
      int a;

      // Inherited via Inflatable
      virtual void Serialize(Deflator const & deflator, std::ostream & stream) override
      {}

      virtual void Restore(std::istream & stream, Inflator & inflator) override
      {}
    };

    TEST_METHOD(Deflator_Basics)
    {
      A a[4];
      Deflator  deflator;

      Assert::IsTrue(deflator.RegisterObject(a[0]) == 0); // first ID from deflator is 0.
      Assert::IsTrue(deflator.RegisterObject(a[0]) == 0); // same object always gets the same ID.
      Assert::IsTrue(deflator.RegisterObject(a[1]) == 1); // next object gets the next ID.
      Assert::IsTrue(deflator.RegisterObject(a[0]) == 0); // same object always gets the same ID.

      deflator.SetNext(10);
      Assert::IsTrue(deflator.RegisterObject(a[2]) == 10); // next object gets next ID.
      Assert::IsTrue(deflator.RegisterObject(a[0]) == 0); // same object always gets the same ID.

      // ID is still valid and same as when registered
      Assert::IsTrue(deflator.GetId(&a[0]) == 0);
      Assert::IsTrue(deflator.GetId(&a[1]) == 1);
      Assert::IsTrue(deflator.GetId(&a[2]) == 10);
      Assert::IsTrue(deflator.GetId(&a[3]) == IdGenerator::kInvalidId); // unregistered object - gets an invalid ID.
      Assert::IsTrue(deflator.GetId(nullptr) == IdGenerator::kInvalidId); // nullptrs always get an an invalid ID.
    }

    TEST_METHOD(Deflator_RegisterInvalid)
    {
      A a;
      Deflator deflator;

      // Registering under reserved invalid ID shall fail.
      deflator.SetNext(IdGenerator::kInvalidId);
      try
      {
        deflator.RegisterObject(a);
        Assert::Fail(L"Registering under invalid ID shall fail.");
      }
      catch (...)
      {
      }
      Assert::IsTrue(deflator.GetId(&a) == IdGenerator::kInvalidId);
    }

    TEST_METHOD(Deflator_IdRangeClash)
    {
      Deflator deflator;
      A a[2];

      // Registration that causes ID range clash shall fail.
      deflator.RegisterObject(a[0]);
      deflator.SetNext(deflator.GetId(&a[0]));
      try
      {
        deflator.RegisterObject(a[1]);
        Assert::IsTrue(false, L"Registration that causes ID range clash shall fail.");
      }
      catch (...)
      {
      }
      Assert::IsTrue(deflator.GetId(&a[1]) == IdGenerator::kInvalidId);
    }
  };
}