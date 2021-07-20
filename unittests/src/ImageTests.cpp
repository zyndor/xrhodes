//
// XRhodes
//
// copyright (c) Gyorgy Straub. All rights reserved.
//
// License: https://github.com/zyndor/xrhodes#License-bsd-2-clause
//
//==============================================================================
#include "FileLifeCycleManager.hpp"

#include "xm.hpp"
#include "xr/Image.hpp"
#include <cstring>

using namespace xr;

namespace
{

class Image
{
private:
  FileLifeCycleManager  flcm;
};

static void Compare(xr::Image const& img, xr::Image const& check)
{
  XM_ASSERT_EQ(check.GetWidth(), img.GetWidth());
  XM_ASSERT_EQ(check.GetHeight(), img.GetHeight());
  XM_ASSERT_EQ(check.GetBytesPerPixel(), img.GetBytesPerPixel());
  XM_ASSERT_NE(check.GetPixelData(), img.GetPixelData());
  XM_ASSERT_TRUE(std::memcmp(check.GetPixelData(), img.GetPixelData(), img.GetPixelDataSize()) == 0);
}

XM_TEST_F(Image, Basic)
{
  xr::Image img;
  XM_ASSERT_EQ(img.GetWidth(), 0);
  XM_ASSERT_EQ(img.GetHeight(), 0);
  XM_ASSERT_EQ(img.GetBytesPerPixel(), 0);
  XM_ASSERT_EQ(img.GetPixelDataSize(), 0);
  XM_ASSERT_EQ(img.GetPixelData(), nullptr);

  Px width = 256;
  Px height = 256;
  uint8_t bpp = 3;
  img.SetSize(width, height, bpp);
  XM_ASSERT_EQ(img.GetWidth(), width);
  XM_ASSERT_EQ(img.GetHeight(), height);
  XM_ASSERT_EQ(img.GetBytesPerPixel(), bpp);
  XM_ASSERT_EQ(img.GetPitch(), width * bpp);
  XM_ASSERT_EQ(img.GetPixelDataSize(), width * height * bpp);
  XM_ASSERT_NE(img.GetPixelData(), nullptr);

  auto p = img.GetPixelData();
  for (Px i = 0; i < height; ++i)
  {
    for (Px j = 0; j < width; ++j)
    {
      *p = uint8_t(i);
      ++p;
      *p = uint8_t(j);
      ++p;
      *p = 0x7f;
      ++p;
    }
  }

  // Png round trip
  XM_ASSERT_TRUE(img.Save("imgTest.png", true));

  xr::Image check;
  XM_ASSERT_TRUE(check.Load("imgTest.png"));
  Compare(img, check);

  // Tga round trip
  XM_ASSERT_TRUE(img.Save("imgTest.tga", true));
  XM_ASSERT_TRUE(check.Load("imgTest.tga"));
  Compare(img, check);
}

}
