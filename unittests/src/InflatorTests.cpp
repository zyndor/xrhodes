#include "stdafx.h"
#include "CppUnitTest.h"
#include <XR/Deflator.hpp>
#include <XR/Inflator.hpp>
#include <XR/streamutils.hpp>
#include <memory>
#include <vector>
#include <algorithm>
#include <iterator>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XR
{
  TEST_CLASS(InflatorTests)
  {
  public:
 
    struct TestObject : public Inflatable
    {
      TestObject const* pOther = nullptr;

      virtual void Serialize(Deflator const & deflator, std::ostream & stream) override
      {}

      virtual void Restore(std::istream & stream, Inflator & inflator) override
      {
        Inflator::IdType  idOther;
        if (ReadBinaryStream(stream, idOther))
        {
          inflator.RegisterMapping(idOther, pOther);
          inflator.RegisterObject(*this);
        }
      }
    };

    TEST_METHOD(Inflator_Basics)
    {
      std::stringstream stream;
      std::vector<Inflator::IdType> ids = {
        0, // reference to itself; might not be practical but not illegal.
        2, 3, 1,  // cyclic dependencies are resolved correctly.
        1, // multiple references to an object are OK
        IdGenerator::kInvalidId // invalid ids are correctly resolved
      };

      for (auto const i : ids)
      {
        WriteBinaryStream(i, stream);
      }
      stream.seekp(0, SEEK_SET);

      Inflator inflator;
      std::vector<TestObject>  testObjects(6);
      std::for_each(testObjects.begin(), testObjects.end(), [&stream, &inflator](TestObject& o)
      {
        o.Restore(stream, inflator);
      });

      inflator.ResolveMappings();

      for (size_t i = 0; i < ids.size(); ++i)
      {
        auto id = ids[i];
        if (id != IdGenerator::kInvalidId)
        {
          Assert::IsTrue(testObjects[i].pOther == &testObjects[id]);
        }
        else
        {
          Assert::IsTrue(testObjects[i].pOther == nullptr);
        }
      }
    }

    TEST_METHOD(Inflator_IdRangeClash)
    {
      TestObject  obj[2];
      Inflator  inflator;
      inflator.SetNext(inflator.RegisterObject(obj[0]));
      
      try
      {
        inflator.RegisterObject(obj[1]);
        Assert::Fail(L"inflator.RegisterObject() must throw for clashing id ranges.");
      }
      catch (std::runtime_error&)
      {}
      catch (...)
      {
        Assert::Fail(L"Wrong exception type");
      }
    }

    TEST_METHOD(Inflator_MappingRegistered)
    {
      TestObject  obj;
      Inflator inflator;
      inflator.RegisterMapping(1000, obj.pOther);

      try
      {
        inflator.ResolveMappings();
        Assert::Fail(L"inflator.ResolveMappings() must throw when attempting to resolve an unregistered object.");
      }
      catch (std::runtime_error&)
      {}
      catch (...)
      {
        Assert::Fail(L"Wrong exception type");
      }
    }
  };
}