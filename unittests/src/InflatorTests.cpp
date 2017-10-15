#include "gtest/gtest.h"
#include "XR/Deflator.hpp"
#include "XR/Inflator.hpp"
#include "XR/streamutils.hpp"
#include <memory>
#include <vector>
#include <algorithm>
#include <iterator>

namespace XR
{
  namespace
  {
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
  }

    TEST(Inflator, Basics)
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
      stream.seekp(0, std::ios_base::beg);

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
          ASSERT_EQ(testObjects[i].pOther, &testObjects[id]);
        }
        else
        {
          ASSERT_EQ(testObjects[i].pOther, nullptr);
        }
      }
    }

    TEST(Inflator, IdRangeClash)
    {
      TestObject  obj[2];
      Inflator  inflator;
      inflator.SetNext(inflator.RegisterObject(obj[0]));
      
      // Clashing ID range.
      ASSERT_THROW(inflator.RegisterObject(obj[1]), std::runtime_error);
    }

    TEST(Inflator, MappingRegistered)
    {
      TestObject  obj;
      Inflator inflator;
      inflator.RegisterMapping(1000, obj.pOther);

      // attempting to resolve unregistered object.
      ASSERT_THROW(inflator.ResolveMappings(), std::runtime_error);
    }
}
