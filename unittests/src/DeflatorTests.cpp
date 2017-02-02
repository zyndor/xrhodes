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

    struct A : public XR::Serializable
    {
      int a;

      // Inherited via Serializable
      virtual void Serialize(Deflator const & deflator, std::ostream & stream) override
      {}

      virtual void Restore(std::istream & stream, Inflator & inflator) override
      {}
    };

    TEST_METHOD(Deflator_Basics)
    {
      A a[5];
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

      // Registering under reserved invalid ID shall fail.
      deflator.SetNext(IdGenerator::kInvalidId);
      try
      {
        deflator.RegisterObject(a[4]);
        Assert::IsTrue(false, L"Registering under invalid ID shall fail.");
      }
      catch (...)
      {
      }
      Assert::IsTrue(deflator.GetId(&a[4]) == IdGenerator::kInvalidId);

      // Registration that causes ID range clash shall fail.
      deflator.SetNext(deflator.GetId(&a[0]));
      try
      {
        deflator.RegisterObject(a[4]);
        Assert::IsTrue(false, L"Registration that causes ID range clash shall fail.");
      }
      catch (...)
      {
      }
      Assert::IsTrue(deflator.GetId(&a[4]) == IdGenerator::kInvalidId);
    }
  };
}