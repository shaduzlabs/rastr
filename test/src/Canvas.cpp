/*
        ##########    Copyright (C) 2018 Vincenzo Pacella
        ##      ##    Distributed under MIT license, see file LICENSE
        ##      ##    or <http://opensource.org/licenses/MIT>
        ##      ##
##########      ############################################################# shaduzlabs.com #####*/

#include <catch.hpp>

#include "CanvasTestFunctions.h"
#include "CanvasTestHelpers.h"

#include <rastr/StaticCanvas.hpp>
#include <rastr/DynamicCanvas.hpp>


#include <iostream>

//--------------------------------------------------------------------------------------------------

namespace sl
{
namespace rastr
{
namespace test
{

//--------------------------------------------------------------------------------------------------

namespace
{

std::string pngFileName(const std::string& test_)
{
  return "test-data/Canvas-" + test_ + ".png";
}

} // namespace

//--------------------------------------------------------------------------------------------------

TEST_CASE("Canvas: constructor", "[gfx][Canvas]")
{
  StaticCanvas<16, 5> c;
  CHECK(c.width() == 16);
  CHECK(c.height() == 5);
  CHECK(c.invalidRegion());
  c.resetInvalidRegion();
  CHECK_FALSE(c.invalidRegion());
  c.setPixel(2,2,Color(0xff));
  CHECK(c.invalidRegion());

  CHECK(c.pixel(2, 2) == Color(0xff));
  CHECK(c.pixel(2000, 2000) == Color());
}

//--------------------------------------------------------------------------------------------------

TEST_CASE("Canvas: lines", "[gfx][Canvas]")
{
  PngCanvas display{128, 128};
  PngCanvas displayFromPng;
  lines(&display);
  std::string fileNameSuffix("lines");
#ifdef DO_WRITE_PICTURES
  REQUIRE_NOTHROW(display.write(pngFileName(fileNameSuffix)));
#endif
  REQUIRE_NOTHROW(displayFromPng.read(pngFileName(fileNameSuffix)));

  CHECK(compare(&display, &displayFromPng));
}

//--------------------------------------------------------------------------------------------------

TEST_CASE("Canvas: circles", "[gfx][Canvas]")
{
  PngCanvas display{128, 128};
  PngCanvas displayFromPng;
  circles(&display);
  std::string fileNameSuffix("circles");
#ifdef DO_WRITE_PICTURES
  REQUIRE_NOTHROW(display.write(pngFileName(fileNameSuffix)));
#endif
  REQUIRE_NOTHROW(displayFromPng.read(pngFileName(fileNameSuffix)));

  CHECK(compare(&display, &displayFromPng));
}

//--------------------------------------------------------------------------------------------------

TEST_CASE("Canvas: triangles", "[gfx][Canvas]")
{
  PngCanvas display{128, 128};
  PngCanvas displayFromPng;
  triangles(&display);
  std::string fileNameSuffix("triangles");
#ifdef DO_WRITE_PICTURES
  REQUIRE_NOTHROW(display.write(pngFileName(fileNameSuffix)));
#endif
  REQUIRE_NOTHROW(displayFromPng.read(pngFileName(fileNameSuffix)));

  CHECK(compare(&display, &displayFromPng));
}

//--------------------------------------------------------------------------------------------------

TEST_CASE("Canvas: rectangles", "[gfx][Canvas]")
{
  PngCanvas display{128, 128};
  PngCanvas displayFromPng;
  rectangles(&display);
  std::string fileNameSuffix("rectangles");

  REQUIRE_NOTHROW(display.write(pngFileName(fileNameSuffix)));
  REQUIRE_NOTHROW(displayFromPng.read(pngFileName(fileNameSuffix)));

  CHECK(compare(&display, &displayFromPng));
}

//--------------------------------------------------------------------------------------------------

TEST_CASE("Canvas: text", "[gfx][Canvas]")
{
  PngCanvas display{128, 128};
  PngCanvas displayFromPng;
  text(&display);
  std::string fileNameSuffix("text");
#ifdef DO_WRITE_PICTURES
  REQUIRE_NOTHROW(display.write(pngFileName(fileNameSuffix)));
#endif
  REQUIRE_NOTHROW(displayFromPng.read(pngFileName(fileNameSuffix)));

  CHECK(compare(&display, &displayFromPng));
}

//--------------------------------------------------------------------------------------------------

TEST_CASE("Canvas: canvas", "[gfx][Canvas]")
{
  PngCanvas display{128, 128};
  PngCanvas displayFromPng;
  canvas(&display);
  std::string fileNameSuffix("canvas");
#ifdef DO_WRITE_PICTURES
  REQUIRE_NOTHROW(display.write(pngFileName(fileNameSuffix)));
#endif
  REQUIRE_NOTHROW(displayFromPng.read(pngFileName(fileNameSuffix)));

  CHECK(compare(&display, &displayFromPng));
}

//--------------------------------------------------------------------------------------------------

TEST_CASE("Canvas: bitmap", "[gfx][Canvas]")
{
  PngCanvas display{128, 128};
  PngCanvas displayFromPng;
  bitmap(&display);
  std::string fileNameSuffix("bitmap");
#ifdef DO_WRITE_PICTURES
  REQUIRE_NOTHROW(display.write(pngFileName(fileNameSuffix)));
#endif
  REQUIRE_NOTHROW(displayFromPng.read(pngFileName(fileNameSuffix)));

  CHECK(compare(&display, &displayFromPng));
}

//--------------------------------------------------------------------------------------------------

} // namespace test
} // namespace rastr
} // namespace sl
