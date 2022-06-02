//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "xm.hpp"
#include "xr/xon/XonRead.hpp"
#include "xr/xon/XonBuildTree.hpp"
#include "xr/utils.hpp"

using namespace xr;

namespace
{

struct Context
{
  std::unique_ptr<XonObject> mRoot;

  Context(char const* xon)
  {
    XonParser::State state;
    mRoot.reset(XonBuildTree(xon, strlen(xon), &state));
  }
};
}

XM_TEST(XonRead, Int)
{
  Context ctx{ "{ 176454, -734581 }" };

  auto& entity = (*ctx.mRoot)[0];
  XM_ASSERT_THROW(XonRead::Number<int8_t>(entity), std::invalid_argument);
  XM_ASSERT_THROW(XonRead::Number<uint8_t>(entity), std::invalid_argument);
  XM_ASSERT_THROW(XonRead::Number<int16_t>(entity), std::invalid_argument);
  XM_ASSERT_THROW(XonRead::Number<uint16_t>(entity), std::invalid_argument);

  XM_ASSERT_EQ(XonRead::Number<int32_t>(entity), 176454);
  XM_ASSERT_EQ(XonRead::Number<uint32_t>(entity), 176454u);
  XM_ASSERT_EQ(XonRead::Number<float>(entity), 176454.f);
  XM_ASSERT_EQ(XonRead::Number<double>(entity), 176454.);

  XM_ASSERT_STREQ(XonRead::CString(entity), "176454");
  XM_ASSERT_EQ(XonRead::String(entity), "176454");

  {
    const std::vector<int32_t> expected = {
      XonRead::Number<int32_t>(entity),
      XonRead::Number<int32_t>((*ctx.mRoot)[1])
    };
    auto result = XonRead::Array<int32_t, XonRead::Number<int32_t> >(*ctx.mRoot);
    XM_ASSERT_TRUE(std::equal(result.begin(), result.end(), expected.begin()));
  }

  auto readDoubleArray = XonRead::Array<double, XonRead::Number<double> >;
  XM_ASSERT_THROW(auto doubles = readDoubleArray(entity), XonEntity::Exception);

  {
    const std::vector<double> expected = {
      XonRead::Number<double>(entity),
      XonRead::Number<double>((*ctx.mRoot)[1])
    };
    auto result = XonRead::Array<double, XonRead::Number<double> >(*ctx.mRoot);
    XM_ASSERT_TRUE(std::equal(result.begin(), result.end(), expected.begin()));
  }
}

XM_TEST(XonRead, Float)
{
  Context ctx{ "{ 1603.7587457924586234863561111111, -2.0 }" };

  auto& entity = (*ctx.mRoot)[0];
  XM_ASSERT_THROW(XonRead::Number<int16_t>(entity), std::invalid_argument);
  XM_ASSERT_THROW(XonRead::Number<uint16_t>(entity), std::invalid_argument);
  XM_ASSERT_THROW(XonRead::Number<int32_t>(entity), std::invalid_argument);
  XM_ASSERT_THROW(XonRead::Number<uint32_t>(entity), std::invalid_argument);

  XM_ASSERT_EQ(XonRead::Number<float>(entity), 1603.7587457924586234863561111111f);
  XM_ASSERT_EQ(XonRead::Number<double>(entity), 1603.7587457924586234863561111111);

  XM_ASSERT_STREQ(XonRead::CString(entity), "1603.7587457924586234863561111111");
  XM_ASSERT_EQ(XonRead::String(entity), "1603.7587457924586234863561111111");

  auto readDoubleArray = XonRead::Array<double, XonRead::Number<double> >;
  XM_ASSERT_THROW(auto doubles = readDoubleArray(entity), XonEntity::Exception);

  {
    const std::vector<double> expected = {
      XonRead::Number<double>(entity),
      XonRead::Number<double>((*ctx.mRoot)[1])
    };
    auto result = XonRead::Array<double, XonRead::Number<double> >(*ctx.mRoot);
    XM_ASSERT_TRUE(std::equal(result.begin(), result.end(), expected.begin()));
  }
}

XM_TEST(XonRead, String)
{
  Context ctx{ "{ salutations, universe }" };

  auto& entity = (*ctx.mRoot)[0];
  XM_ASSERT_THROW(XonRead::Number<int32_t>(entity), std::invalid_argument);
  XM_ASSERT_THROW(XonRead::Number<uint32_t>(entity), std::invalid_argument);
  XM_ASSERT_THROW(XonRead::Number<float>(entity), std::invalid_argument);
  XM_ASSERT_THROW(XonRead::Number<double>(entity), std::invalid_argument);

  XM_ASSERT_STREQ(XonRead::CString(entity), "salutations");
  XM_ASSERT_EQ(XonRead::String(entity), "salutations");

  auto readDoubleArray = XonRead::Array<double, XonRead::Number<double> >;
  XM_ASSERT_THROW(auto doubles = readDoubleArray(entity), XonEntity::Exception);
  XM_ASSERT_THROW(auto doubles = readDoubleArray(*ctx.mRoot), std::invalid_argument);

  std::vector<std::string> expected{
    XonRead::CString(entity),
    XonRead::CString((*ctx.mRoot)[1])
  };
  auto result = XonRead::Array<const char*, XonRead::CString>(*ctx.mRoot);
  XM_ASSERT_TRUE(std::equal(result.begin(), result.end(), expected.begin()));
}

namespace
{

struct Order
{
  struct Item
  {
    uint64_t id;
    uint32_t quantity;

    float fudgeFactor;
  };

  std::vector<Item> items;

  std::string recipientName;
  uint32_t countryCode;
  uint32_t houseNo;
  std::string street;
  std::string zipCode;

  float fudgeFactor;
};

auto kOrderItemReader = std::move(XonReader<Order::Item>()
  .Register("id", Structured::MakeProperty<Order::Item>(XonRead::Number<uint64_t>, &Order::Item::id), Structured::REQUIRED)
  .Register("quantity", Structured::MakeProperty<Order::Item>(XonRead::Number<uint32_t>, &Order::Item::quantity), Structured::REQUIRED)
  .Register("fudge", Structured::MakeProperty<Order::Item>(XonRead::Number<float>, &Order::Item::fudgeFactor))
);

auto kOrderReader = std::move(XonReader<Order>()
  .Register("items", Structured::MakeProperty<Order>(
    XonRead::Array<Order::Item, decltype(kOrderItemReader)::Read>,
    &Order::items))
  .Register("recipientName", Structured::MakeProperty<Order>(XonRead::String, &Order::recipientName))
  .Register("countryCode", Structured::MakeProperty<Order>(XonRead::Number<uint32_t>, &Order::countryCode))
  .Register("houseNo", Structured::MakeProperty<Order>(XonRead::Number<uint32_t>, &Order::houseNo))
  .Register("street", Structured::MakeProperty<Order>(XonRead::String, &Order::street))
  .Register("zip", Structured::MakeProperty<Order>(XonRead::String, &Order::zipCode))
  .Register("fudge", Structured::MakeProperty<Order>(XonRead::Number<float>, &Order::fudgeFactor))
);

}

XM_TEST(XonRead, Reader)
{
  Context ctx{ "{ items: { { id: 274634674, quantity: 5428, fudge: 0.5 },"
    "{ id: 653764, quantity: 361, fudge: 0.25 } },"
    "recipientName: \"Johan Fucek\", countryCode: 58, houseNo: 126,"
    "street: Gallbladderstrasse, zip: 1828-B, fudge: 9000 }" };

  kOrderItemReader.SetInstance();

  Order order;
  kOrderReader.Process(*ctx.mRoot, order);

  XM_ASSERT_EQ(order.items.size(), 2u);
  XM_ASSERT_EQ(order.items[0].id, 274634674);
  XM_ASSERT_EQ(order.items[0].quantity, 5428u);
  XM_ASSERT_EQ(order.items[0].fudgeFactor, .5f);
  XM_ASSERT_EQ(order.items[1].id, 653764);
  XM_ASSERT_EQ(order.items[1].quantity, 361u);
  XM_ASSERT_EQ(order.items[1].fudgeFactor, .25f);

  XM_ASSERT_EQ(order.recipientName, "Johan Fucek");
  XM_ASSERT_EQ(order.countryCode, 58u);
  XM_ASSERT_EQ(order.houseNo, 126u);
  XM_ASSERT_EQ(order.street, "Gallbladderstrasse");
  XM_ASSERT_EQ(order.zipCode, "1828-B");
  XM_ASSERT_EQ(order.fudgeFactor, 9000.f);
}
