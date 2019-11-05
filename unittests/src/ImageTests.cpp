//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "FileLifeCycleManager.hpp"

#include "gtest/gtest.h"
#include "xr/Image.hpp"
#include <cstring>

class ImageTests : public ::testing::Test
{
private:
  xr::FileLifeCycleManager  flcm;
};

static void Compare(xr::Image const& img, xr::Image const& check)
{
  ASSERT_EQ(check.GetWidth(), img.GetWidth());
  ASSERT_EQ(check.GetHeight(), img.GetHeight());
  ASSERT_EQ(check.GetBytesPerPixel(), img.GetBytesPerPixel());
  ASSERT_NE(check.GetPixelData(), img.GetPixelData());
  ASSERT_TRUE(std::memcmp(check.GetPixelData(), img.GetPixelData(), img.GetPixelDataSize()) == 0);
}

TEST_F(ImageTests, Basic)
{
  xr::Image img;
  ASSERT_EQ(img.GetWidth(), 0);
  ASSERT_EQ(img.GetHeight(), 0);
  ASSERT_EQ(img.GetBytesPerPixel(), 0);
  ASSERT_EQ(img.GetPixelDataSize(), 0);
  ASSERT_EQ(img.GetPixelData(), nullptr);

  uint32_t width = 256;
  uint32_t height = 256;
  uint32_t bpp = 3;
  img.SetSize(width, height, bpp);
  ASSERT_EQ(img.GetWidth(), width);
  ASSERT_EQ(img.GetHeight(), height);
  ASSERT_EQ(img.GetBytesPerPixel(), bpp);
  ASSERT_EQ(img.GetPitch(), width * bpp);
  ASSERT_EQ(img.GetPixelDataSize(), width * height * bpp);
  ASSERT_NE(img.GetPixelData(), nullptr);

  auto p = img.GetPixelData();
  for (uint32_t i = 0; i < height; ++i)
  {
    for (uint32_t j = 0; j < width; ++j)
    {
      *p = i;
      ++p;
      *p = j;
      ++p;
      *p = 0x7f;
      ++p;
    }
  }

  // Png round trip
  ASSERT_TRUE(img.Save("imgTest.png", true));

  xr::Image check;
  ASSERT_TRUE(check.Load("imgTest.png"));
  Compare(img, check);

  // Tga round trip
  ASSERT_TRUE(img.Save("imgTest.tga", true));
  ASSERT_TRUE(check.Load("imgTest.tga"));
  Compare(img, check);
}
