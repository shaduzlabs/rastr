/*
        ##########    Copyright (C) 2017 Vincenzo Pacella
        ##      ##    Distributed under MIT license, see file LICENSE
        ##      ##    or <http://opensource.org/licenses/MIT>
        ##      ##
##########      ############################################################# shaduzlabs.com #####*/

#include <rastr.h>

#include <freetype/ftglyph.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <clara.hpp>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
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

struct Character
{
  uint8_t width;
  uint8_t height;
  int8_t xOffset;
  int8_t yOffset;
  uint8_t xAdvance;
  std::vector<uint8_t> data;
};

//--------------------------------------------------------------------------------------------------

using Characters = std::map<unsigned, Character>;

//--------------------------------------------------------------------------------------------------

class Font
{
public:
  enum class RenderMode
  {
    monochrome,
    grayscale,
  };

  Font(const std::string& path_,
    unsigned height_,
    unsigned firstCharacter_,
    unsigned lastCharacter_,
    RenderMode mode_)
    : m_size(height_)
    , m_lineHeight(height_)
    , m_firstCharacter(firstCharacter_)
    , m_lastCharacter(lastCharacter_)
    , m_renderMode(mode_)
  {
    auto error = FT_Init_FreeType(&m_library);
    if (error)
    {
      throw std::runtime_error("Could not initialize FreeType");
    }
    error = FT_New_Face(m_library, path_.c_str(), 0, &m_face);
    if (error == FT_Err_Unknown_File_Format)
    {
      throw std::runtime_error("FreeType: Unknown file format");
    }
    else if (error)
    {
      const std::string errorString = "FreeType: error #" + std::to_string(error);
      throw std::runtime_error(errorString);
    }

    FT_Set_Pixel_Sizes(m_face, 0, height_);

    m_name = m_face->family_name;
    m_style = m_face->style_name;

    for (unsigned i = m_firstCharacter; i <= m_lastCharacter; ++i)
    {
      character(i);
    }
  }

  const std::string& name() const
  {
    return m_name;
  }
  const std::string& style() const
  {
    return m_style;
  }

  unsigned size() const
  {
    return m_size;
  }
  unsigned lineHeight() const
  {
    return m_lineHeight;
  }
  unsigned firstCharacter() const
  {
    return m_firstCharacter;
  }
  unsigned lastCharacter() const
  {
    return m_lastCharacter;
  }
  RenderMode renderMode() const
  {
    return m_renderMode;
  }

  const Characters& characters() const
  {
    return m_characters;
  }

private:
  void character(unsigned c_)
  {
    if (!m_face)
    {
      throw std::runtime_error("FreeType: the font hasn't been initialized");
    }

    FT_Int32 loadFlags
      = FT_LOAD_RENDER
        | (m_renderMode == RenderMode::monochrome ? FT_LOAD_MONOCHROME | FT_LOAD_TARGET_MONO
                                                  : FT_LOAD_TARGET_NORMAL);
    if (FT_Load_Char(m_face, c_, loadFlags))
    {
      throw std::runtime_error("FreeType: FT_Load_Char error ");
    }

    if (FT_Render_Glyph(m_face->glyph,
          (m_renderMode == RenderMode::monochrome ? FT_RENDER_MODE_MONO : FT_RENDER_MODE_NORMAL)))
    {
      throw std::runtime_error("FreeType: FT_Render_Glyph error ");
    }

    auto& bitmap = m_face->glyph->bitmap;
    const auto bitmap_left = m_face->glyph->bitmap_left;
    const auto bitmap_top = m_face->glyph->bitmap_top;
    const auto nBytes = static_cast<std::size_t>(std::ceil(
      (bitmap.width * bitmap.rows) / (m_renderMode == RenderMode::monochrome ? 8.f : 1.f)));

    Character character;
    character.data.resize(nBytes, 0);
    character.width = bitmap.width;
    character.height = bitmap.rows;
    character.xOffset = bitmap_left;
    character.yOffset = bitmap_top;
    character.xAdvance = m_face->glyph->linearHoriAdvance / 65536;

    if (character.height > m_lineHeight)
    {
      m_lineHeight = character.height;
    }

    unsigned bitIndex = 0;
    for (unsigned y = 0; y < bitmap.rows; ++y)
    {
      for (unsigned x = 0; x < bitmap.width; ++x)
      {
        if (m_renderMode == RenderMode::monochrome)
        {
          const auto p = x >> 3;
          const auto offset = (p * 8) + (7 - x);
          const auto index = (y * bitmap.pitch) + p;
          if ((bitmap.buffer[index] >> offset) & 0x01)
          {
            character.data[bitIndex / 8] |= 0x80 >> (bitIndex % 8);
          }
          ++bitIndex;
        }
        else
        {
          const auto index = (y * bitmap.pitch) + x;
          character.data[index] |= bitmap.buffer[index];
        }
      }
    }

    m_characters.emplace(c_, character);
  }

  FT_Library m_library{nullptr};
  FT_Face m_face{nullptr};

  std::string m_name;
  std::string m_style;
  unsigned m_size;
  unsigned m_lineHeight;
  unsigned m_firstCharacter;
  unsigned m_lastCharacter;
  RenderMode m_renderMode;

  Characters m_characters;
};

//--------------------------------------------------------------------------------------------------

std::string to_string(const Font& font_)
{
  std::ostringstream ss;
  std::map<int, std::string> charactersMap;

  const std::string separator
    = "//"
      "------------------------------------------------------------------------------------------"
      "--------";
  ss << separator << std::endl;
  ss << "// Font: " << font_.name() << std::endl;
  ss << "// Style: " << font_.style() << std::endl;
  ss << "// Size: " + std::to_string(font_.size()) << std::endl;
  ss << "// Line height: " + std::to_string(font_.lineHeight()) << std::endl;
  ss << "// Number of chars: " + std::to_string(font_.characters().size()) << std::endl;
  ss << "// Bits per pixel: " << (font_.renderMode() == Font::RenderMode::monochrome ? 1 : 8)
     << std::endl;
  ss << "//"
        "------------------------------------------------------------------------------------------"
        "--------"
     << std::endl;
  ss << std::endl;

  ss << "#ifdef ARDUINO" << std::endl;
  ss << "#include <avr/pgmspace.h>" << std::endl;
  ss << "#else" << std::endl;
  ss << "#define PROGMEM" << std::endl;
  ss << "#endif" << std::endl;
  ss << std::endl;

  auto resolution = font_.renderMode() == Font::RenderMode::grayscale ? "8bpp" : "1bpp";
  auto fontVariableName = "k_font_" + variableName(font_.name()) + "_" + font_.style() + "_"
                          + std::to_string(font_.size()) + "_" + resolution;
  auto fontCharsVariableName = fontVariableName + "_chars";

  ss << "// clang-format off " << std::endl;
  ss << std::endl;

  ss << "// bitmap data" << std::endl;

  for (auto& character : font_.characters())
  {
    unsigned nBytes = character.second.data.size();

    auto characterVariableName = fontVariableName + "_c" + std::to_string((int)character.first);

    if (nBytes > 0)
    {
      std::string separator;
      ss << "const PROGMEM uint8_t " << characterVariableName << "[] = { ";
      for (auto i = 0; i < nBytes; ++i)
      {
        ss << separator << "0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex
           << (int)character.second.data[i];
        separator = ", ";
      }
      ss << " }; " << std::endl;

      charactersMap.emplace(character.first, "&" + characterVariableName + "[0]");
    }
  }

  ss << std::endl;

  ss << "// character data" << std::endl;
  ss << "const PROGMEM sl::rastr::Character " + fontCharsVariableName + "[] = { " << std::endl;
  ss << "//                     W      H    XOff   YOff   XAdv   Data" << std::endl;

  for (const auto& c : font_.characters())
  {

    ss << "  /* '" << (char)c.first << "' (0x" << std::uppercase << std::setfill('0')
       << std::setw(2) << std::hex << (int)c.first << ") */ {" << std::dec;

    ss << std::setfill(' ') << std::setw(5) << (int)c.second.width << ", ";
    ss << std::setfill(' ') << std::setw(5) << (int)c.second.height << ", ";
    ss << std::setfill(' ') << std::setw(5) << (int)c.second.xOffset << ", ";
    ss << std::setfill(' ') << std::setw(5) << (int)c.second.yOffset << ", ";
    ss << std::setfill(' ') << std::setw(5) << (int)c.second.xAdvance << ", ";

    const auto& cVariableName = charactersMap.find(c.first);
    if (cVariableName != charactersMap.end())
    {
      ss << cVariableName->second;
    }
    else
    {
      ss << "nullptr";
    }
    ss << " }, " << std::endl;
  }

  ss << "};" << std::endl;
  ss << "" << std::endl;

  ss << "// font" << std::endl;
  ss << "const PROGMEM sl::rastr::Font " + fontVariableName + " = {" << std::endl;
  {
    ss << "  /* name and size:   */ { \"" << font_.name() << "\", " << (int)font_.size() << " },"
       << std::endl;
    ss << "  /* bits/pixel:      */ "
       << (font_.renderMode() == Font::RenderMode::monochrome ? 1 : 8) << "," << std::endl;
    ss << "  /* line height:     */ " << (int)font_.lineHeight() << "," << std::endl;
    ss << "  /* first character: */ " << (int)font_.firstCharacter() << "," << std::endl;
    ss << "  /* last character:  */ " << (int)font_.lastCharacter() << ", " << std::endl;
    ss << "  &" << fontCharsVariableName << "[0]" << std::endl;
    ss << "};" << std::endl;
  }
  ss << "" << std::endl;
  ss << "// clang-format on " << std::endl;

  ss << std::endl;

  return ss.str();
}

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

  std::cout << "TrueType font converter utility" << std::endl << std::endl;

  bool showHelp{false};
  boost::filesystem::path fontPath;
  boost::filesystem::path outputPath;
  unsigned fontSize = 0;
  unsigned firstChar = 32;
  unsigned lastChar = 126;
  bool grayscale{false};

  auto cli = Help(showHelp) | Opt(fontPath, "TrueType Font")["-t"]["--ttf"]("the font file")
             | Opt(fontSize, "Font size")["-s"]["--size"]("font size")
             | Opt(firstChar, "First character")["-f"]["--first"]("first character (default: 32)")
             | Opt(lastChar, "Last character")["-l"]["--last"]("last character (default: 126)")
             | Opt(grayscale, "Grayscale")["-g"]["--grayscale"](
                 "render characters using 8 bits per pixel (default: 1 bit per pixel)")
             | Opt(outputPath, "Output file")["-o"]["--output"]("output file name");

  auto result = cli.parse(Args(argc_, argv_));
  if (!result)
  {
    std::cerr << " * Error: " << result.errorMessage() << std::endl << std::endl;
    return -1;
  }

  if (showHelp)
  {
    std::cout << cli << std::endl << std::endl;
    return 0;
  }
  if (fontPath.empty() || fontSize == 0)
  {
    std::cerr << " * Error: : you must specify at least a path to a font and the size"
              << std::endl
              << std::endl;
    return -2;
  }

  const auto renderMode = grayscale ? Font::RenderMode::grayscale : Font::RenderMode::monochrome;
  try
  {
    Font font(fontPath.string(), fontSize, firstChar, lastChar, renderMode);
    auto resolution = font.renderMode() == Font::RenderMode::grayscale ? "8bpp" : "1bpp";

    if (outputPath.empty())
    {
      outputPath = fontPath.stem().string() + "_" + variableName(font.style()) + "_"
                   + std::to_string(font.size()) + "_" + resolution + ".h";
    }
    else if (!outputPath.parent_path().empty()
             && !boost::filesystem::exists(outputPath.parent_path()))
    {
      std::cerr << " * Error: The output path " << outputPath.parent_path() << " does not exist" << std::endl
                << std::endl;
      return -3;
    }
    std::cout << "Output file: " << outputPath.string() << std::endl;

    boost::filesystem::ofstream ofs{outputPath};
    ofs << to_string(font);
  }
  catch (const std::exception& ex)
  {
    std::cerr << " * Error: " << ex.what() << "\n";
    return -4;
  }

  return 0;
}

//--------------------------------------------------------------------------------------------------
