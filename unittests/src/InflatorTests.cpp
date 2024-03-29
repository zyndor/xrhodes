//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xm.hpp"
#include "xr/io/Deflator.hpp"
#include "xr/io/Inflator.hpp"
#include "xr/io/streamutils.hpp"
#include <memory>
#include <vector>
#include <algorithm>
#include <iterator>
#include <sstream>
using namespace xr;

namespace
{

struct TestObject : public Inflatable
{
  TestObject const* pOther = nullptr;

  virtual void Serialize(Deflator const& /*deflator*/, std::ostream& /*stream*/) override
  {}

  virtual void Restore(std::istream& stream, Inflator & inflator) override
  {
    Inflator::IdType  idOther;
    if (ReadBinaryStream(stream, idOther))
    {
      inflator.RegisterMapping(idOther, pOther);
      inflator.RegisterObject(*this);
    }
  }
};

XM_TEST(Inflator, Basics)
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
      XM_ASSERT_EQ(testObjects[i].pOther, &testObjects[id]);
    }
    else
    {
      XM_ASSERT_EQ(testObjects[i].pOther, nullptr);
    }
  }
}

XM_TEST(Inflator, IdRangeClash)
{
  TestObject  obj[2];
  Inflator  inflator;
  inflator.SetNext(inflator.RegisterObject(obj[0]));

  // Clashing ID range.
  XM_ASSERT_THROW(inflator.RegisterObject(obj[1]), std::runtime_error);
}

XM_TEST(Inflator, MappingRegistered)
{
  TestObject  obj;
  Inflator inflator;
  inflator.RegisterMapping(1000, obj.pOther);

  // attempting to resolve unregistered object.
  XM_ASSERT_THROW(inflator.ResolveMappings(), std::runtime_error);
}

}
