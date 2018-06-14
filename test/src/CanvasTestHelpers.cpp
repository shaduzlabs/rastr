/*
        ##########    Copyright (C) 2018 Vincenzo Pacella
        ##      ##    Distributed under MIT license, see file LICENSE
        ##      ##    or <http://opensource.org/licenses/MIT>
        ##      ##
##########      ############################################################# shaduzlabs.com #####*/

#include "CanvasTestHelpers.h"

#include <iostream>

#define PNG_DEBUG 3
#include <png.h>


//--------------------------------------------------------------------------------------------------

namespace sl
{
namespace rastr
{
namespace test
{

//--------------------------------------------------------------------------------------------------

PngCanvas::PngCanvas() : DynamicCanvas(0, 0)
{
}

//--------------------------------------------------------------------------------------------------

PngCanvas::PngCanvas(unsigned w_, unsigned h_, unsigned size_)
  : DynamicCanvas(w_, h_, size_)
{
}

//--------------------------------------------------------------------------------------------------

void PngCanvas::read(const std::string& fileName_)
{
  unsigned char header[8]; // 8 is the maximum size that can be checked

  FILE* fp = fopen(fileName_.c_str(), "rb");
  if (!fp)
  {
    throw std::runtime_error(fileName_ + " could not be opened for reading");
  }
  fread(header, 1, 8, fp);
  if (png_sig_cmp(static_cast<png_const_bytep>(header), 0, 8))
  {
    throw std::runtime_error(fileName_ + " is not recognized as a PNG file");
  }

  auto png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if (!png_ptr)
  {
    throw std::runtime_error("png_create_read_struct failed");
  }
  auto info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
  {
    throw std::runtime_error("png_create_info_struct failed");
  }
  if (setjmp(png_jmpbuf(png_ptr)))
  {
    throw std::runtime_error("Error during init_io");
  }

  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, 8);

  png_read_info(png_ptr, info_ptr);

  const auto width = png_get_image_width(png_ptr, info_ptr);
  const auto height = png_get_image_height(png_ptr, info_ptr);
  png_read_update_info(png_ptr, info_ptr);

  if (setjmp(png_jmpbuf(png_ptr)))
  {
    throw std::runtime_error("Error while reading image");
  }

  auto* rows = (png_bytep*)malloc(sizeof(png_bytep) * height);
  const auto bytesPerRow = png_get_rowbytes(png_ptr, info_ptr);
  resize(width, height, bytesPerRow * height);

  for (auto y = 0; y < height; y++)
  {
    rows[y] = data() + (y * bytesPerRow);
  }

  png_read_image(png_ptr, &rows[0]);

  free(rows);

  fclose(fp);
}

//--------------------------------------------------------------------------------------------------

void PngCanvas::write(const std::string& fileName_)
{
  FILE* fp = fopen(fileName_.c_str(), "wb");
  if (!fp)
    abort();

  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png)
    abort();

  png_infop info = png_create_info_struct(png);
  if (!info)
    abort();

  if (setjmp(png_jmpbuf(png)))
    abort();

  png_init_io(png, fp);

  png_set_IHDR(png,
    info,
    width(),
    height(),
    8,
    PNG_COLOR_TYPE_RGB,
    PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_DEFAULT);
  png_write_info(png, info);

  auto* rows = (png_bytep*)malloc(sizeof(png_bytep) * height());
  const auto bytesPerRow = png_get_rowbytes(png, info);

  for (auto y = 0; y < height(); y++)
  {
    rows[y] = data() + (bytesPerRow * y);
  }

  png_write_image(png, rows);
  png_write_end(png, NULL);

  free(rows);

  fclose(fp);
}

//--------------------------------------------------------------------------------------------------

bool matchColorForAllPixels(const Canvas* canvas_, const Color& color_)
{
  for (unsigned x = 0U; x < canvas_->width(); x++)
  {
    for (unsigned y = 0U; y < canvas_->height(); y++)
    {
      if (color_ != canvas_->pixel(x, y))
      {
        return false;
      }
    }
  }
  return true;
}

//--------------------------------------------------------------------------------------------------

bool compare(const Canvas* c1_, const Canvas* c2_)
{
  if (c1_->width() != c2_->width() || c1_->height() != c2_->height())
  {
    return false;
  }
  for (unsigned x = 0U; x < c1_->width(); x++)
  {
    for (unsigned y = 0U; y < c1_->height(); y++)
    {
      if (c1_->pixel(x, y) != c2_->pixel(x, y))
      {
        return false;
      }
    }
  }

  return true;
}

//--------------------------------------------------------------------------------------------------

} // namespace test
} // namespace rastr
} // namespace sl
