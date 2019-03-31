/*
        ##########    Copyright (C) 2018 Vincenzo Pacella
        ##      ##    Distributed under MIT license, see file LICENSE
        ##      ##    or <http://opensource.org/licenses/MIT>
        ##      ##
##########      ############################################################# shaduzlabs.com #####*/

#include <rastr.h>

#define PNG_DEBUG 3
#include <png.h>

#include <clara.hpp>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

//--------------------------------------------------------------------------------------------------

std::string variableName(std::string str)
{
  str.erase(
    std::remove_if(
      str.begin(), str.end(), [](unsigned char const& c) -> bool { return !std::isalnum(c); }),
    str.end());

  return str;
}

//--------------------------------------------------------------------------------------------------

class Picture
{
public:
  Picture(const std::string& filename)
  {
    unsigned char header[8]; // 8 is the maximum size that can be checked

    FILE* fp = fopen(filename.c_str(), "rb");
    if (!fp)
    {
      throw std::runtime_error(filename + " could not be opened for reading");
    }
    fread(header, 1, 8, fp);
    if (png_sig_cmp(static_cast<png_const_bytep>(header), 0, 8))
    {
      throw std::runtime_error(filename + " is not recognized as a PNG file");
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

    m_width = png_get_image_width(png_ptr, info_ptr);
    m_height = png_get_image_height(png_ptr, info_ptr);
    m_colorType = png_get_color_type(png_ptr, info_ptr);
    m_bitDepth = png_get_bit_depth(png_ptr, info_ptr);

    m_numberOfPasses = png_set_interlace_handling(png_ptr);
    png_read_update_info(png_ptr, info_ptr);

    /* read file */
    if (setjmp(png_jmpbuf(png_ptr)))
    {
      throw std::runtime_error("Error while reading image");
    }

    m_rows = (png_bytep*)malloc(sizeof(png_bytep) * m_height);
    auto bytesPerRow = png_get_rowbytes(png_ptr, info_ptr);
    for (auto y = 0; y < m_height; y++)
    {
      m_rows[y] = (png_byte*)malloc(bytesPerRow);
    }

    png_read_image(png_ptr, &m_rows[0]);

    fclose(fp);
  }

  ~Picture()
  {
    if (!m_rows)
    {
      for (auto y = 0; y < m_height; y++)
      {
        free(m_rows[y]);
      }
      free(m_rows);
    }
  }

  unsigned width() const
  {
    return m_width;
  }
  unsigned height() const
  {
    return m_height;
  }

  bool at(unsigned x, unsigned y) const
  {
    if (!m_rows)
    {
      throw std::runtime_error("Invalid image data");
    }
    if (x >= m_width || y >= m_height)
    {
      throw std::runtime_error("The requested pixel is out of the image boundaries");
    }
    auto* row = m_rows[y];
    auto* pixel = &(row[x * 4]);

    return pixel[3] > 0;
  }

private:
  png_uint_32 m_width;
  png_uint_32 m_height;
  png_byte m_colorType;
  png_byte m_bitDepth;
  int m_numberOfPasses;

  png_bytep* m_rows;
};

//--------------------------------------------------------------------------------------------------

int main(int argc_, char* argv_[])
{
  using namespace clara;

  std::string version(std::to_string(RASTR_VERSION_MAJOR) + "."
                      + std::to_string(RASTR_VERSION_MINOR) + "."
                      + std::to_string(RASTR_VERSION_MICRO));

  std::cout << std::endl;
  std::cout << ",-.  ,-.  ,-.  |-  ,-. " << std::endl;
  std::cout << "|    ,-|  `-.  |   |   " << std::endl;
  std::cout << "'    `-^  `-'  `'  '   " << std::endl;

  std::cout << "             v. " << version << std::endl << std::endl;

  std::cout << "Png converter utility" << std::endl << std::endl;

  bool showHelp{false};
  bool grayscale{false};
  boost::filesystem::path pngPath;
  boost::filesystem::path outputPath;

  auto cli = Help(showHelp) | Opt(pngPath, "PNG file")["-p"]["--png"]("input png file")
             | Opt(outputPath, "Output file")["-o"]["--output"]("output file name")
             | Opt(grayscale)["-g"]["--grayscale"](
                 "render the bitmap using 8 bits per pixel (default: 1 bit per pixel)");

  auto result = cli.parse(Args(argc_, argv_));
  if (!result)
  {
    std::cerr << " * Error: " << result.errorMessage() << std::endl << std::endl;
    exit(1);
  }

  if (showHelp)
  {
    std::cout << cli << std::endl << std::endl;
    return 0;
  }

  if (pngPath.empty())
  {
    std::cerr << " * Error: : you must specify a path to a png file" << std::endl << std::endl;
    exit(2);
  }

  try
  {
    if (outputPath.empty())
    {
      outputPath = pngPath.stem().string() + ".h";
    }
    else if (!outputPath.parent_path().empty()
             && !boost::filesystem::exists(outputPath.parent_path()))
    {
      std::cerr << " * Error: The output path " << outputPath.parent_path() << " does not exist"
                << std::endl
                << std::endl;
      exit(3);
    }
    std::cout << "Output file: " << outputPath.string() << std::endl;

    std::vector<std::string> rows;

    Picture picture(pngPath.string());
    rows.emplace_back("//"
                      "--------------------------------------------------------"
                      "--------------------"
                      "----------------------");
    rows.emplace_back("// Original file name: " + pngPath.filename().string());
    rows.emplace_back(
      "// Size: " + std::to_string(picture.width()) + "x" + std::to_string(picture.height()));
    rows.emplace_back("// Bits per pixel: " + std::to_string(grayscale ? 8 : 1));

    rows.emplace_back("//"
                      "--------------------------------------------------------"
                      "--------------------"
                      "----------------------");
    rows.emplace_back("");

    rows.emplace_back("#ifdef ARDUINO");
    rows.emplace_back("#include <avr/pgmspace.h>");
    rows.emplace_back("#else");
    rows.emplace_back("#define PROGMEM");
    rows.emplace_back("#endif");
    rows.emplace_back("");

    auto bitmapVariableName = "k_bitmap_" + variableName(pngPath.stem().string());
    auto bitmapDataVariableName = bitmapVariableName + "_data";

    rows.emplace_back("// clang-format off ");
    rows.emplace_back("");

    std::vector<bool> pixels;
    for (auto y = 0; y < picture.height(); ++y)
    {
      for (auto x = 0; x < picture.width(); ++x)
      {
        pixels.push_back(picture.at(x, y));
      }
    }

    unsigned nBytes = static_cast<unsigned>(std::ceil(pixels.size() / 8.f));
    std::vector<uint8_t> bytes(nBytes);

    if (nBytes > 0)
    {
      rows.emplace_back("// bitmap data");
      std::ostringstream ss;
      ss << "const PROGMEM uint8_t " << bitmapDataVariableName << "[] = { ";
      std::string separator;
      ss << "\n  ";
      for (auto i = 0; i < nBytes; ++i)
      {
        for (auto b = 0; b < 8; ++b)
        {
          if (pixels[i * 8 + b])
          {
            bytes[i] |= (1 << (7 - b));
          }
          else
          {
          }
        }
        ss << separator << ((i % 16 == 0 && i != 0) ? "\n  " : "") << "0x" << std::uppercase
           << std::setfill('0') << std::setw(2) << std::hex << (int)bytes[i];
        separator = ", ";
      }
      ss << "\n};\n";
      rows.emplace_back(ss.str());
    }

    rows.emplace_back("const PROGMEM sl::rastr::Bitmap " + bitmapVariableName + " = {");

    {
      std::ostringstream ss;
      ss << "  /* width:          */ " << (int)picture.width() << ",\n";
      ss << "  /* height:         */ " << (int)picture.height() << ",\n";
      ss << "  /* bits per pixel: */ " << (int)(grayscale ? 8 : 1) << ",\n";
      ss << "  /* data:           */ &" << bitmapDataVariableName << "[0]";
      rows.emplace_back(ss.str());
    }

    rows.emplace_back("};");
    rows.emplace_back("");
    rows.emplace_back("// clang-format on ");

    rows.emplace_back("");

    rows.emplace_back("//"
                      "--------------------------------------------------------"
                      "--------------------"
                      "----------------------");

    boost::filesystem::ofstream ofs{outputPath};
    for (const auto& row : rows)
    {
      ofs << row << std::endl;
    }
  }
  catch (const std::exception& ex)
  {
    std::cerr << " * Error: " << ex.what() << "\n";
    return -10;
  }

  return 0;
}

//--------------------------------------------------------------------------------------------------
