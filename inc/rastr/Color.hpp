/*
        ##########    Copyright (C) 2017 Vincenzo Pacella
        ##      ##    Distributed under MIT license, see file LICENSE
        ##      ##    or <http://opensource.org/licenses/MIT>
        ##      ##
##########      ############################################################# shaduzlabs.com #####*/

#pragma once

// -------------------------------------------------------------------------------------------------

#include <rastr/Types.hpp>
#include <rastr/Util.hpp>

#include <cmath>

// -------------------------------------------------------------------------------------------------

namespace sl
{
namespace rastr
{

// -------------------------------------------------------------------------------------------------

class Color
{
public:
  //------------------------------------------------------------------------------------------------

  Color() : m_blendMode(BlendMode::Transparent)
  {
  }

  //------------------------------------------------------------------------------------------------

  explicit Color(const BlendMode blendMode_) : m_blendMode(blendMode_)
  {
  }

  //------------------------------------------------------------------------------------------------

  explicit Color(const uint8_t mono_) : Color(mono_, mono_, mono_, 0xFF)
  {
  }

  //------------------------------------------------------------------------------------------------

  Color(const Color& color_, const uint8_t level_) : m_alpha(level_)
  {
    m_red = color_.red() == 0 ? 0 : static_cast<uint8_t>(level_ * (color_.red()) >> 8);
    m_green = color_.green() == 0 ? 0 : static_cast<uint8_t>(level_ * (color_.green()) >> 8);
    m_blue = color_.blue() == 0 ? 0 : static_cast<uint8_t>(level_ * (color_.blue()) >> 8);
  }

  //------------------------------------------------------------------------------------------------

  Color(const uint8_t red_, uint8_t const green_, const uint8_t blue_)
    : Color(red_, green_, blue_, 0xFF)
  {
  }

  //------------------------------------------------------------------------------------------------

  Color(const uint8_t red_, const uint8_t green_, const uint8_t blue_, const uint8_t alpha_)
    : m_red(red_)
    , m_green(green_)
    , m_blue(blue_)
    , m_alpha(alpha_)
    , m_mono(util::max<uint8_t>(red_, green_, blue_)) // Max decomposition: take the highest value
  {
  }

  //------------------------------------------------------------------------------------------------

  uint8_t red() const
  {
    return (m_blendMode == BlendMode::Invert) ? ~m_red : m_red;
  }

  //------------------------------------------------------------------------------------------------

  uint8_t green() const
  {
    return (m_blendMode == BlendMode::Invert) ? ~m_green : m_green;
  }

  //------------------------------------------------------------------------------------------------

  uint8_t blue() const
  {
    return (m_blendMode == BlendMode::Invert) ? ~m_blue : m_blue;
  }

  //------------------------------------------------------------------------------------------------

  uint8_t alpha() const
  {
    return m_alpha;
  }

  //------------------------------------------------------------------------------------------------

  uint8_t mono() const
  {
    return m_mono;
  }

  //------------------------------------------------------------------------------------------------

  bool active() const
  {
    return util::max<uint8_t>(m_red, m_green, m_blue) > 127;
  }
  //------------------------------------------------------------------------------------------------

  BlendMode blendMode() const
  {
    return m_blendMode;
  }
  //------------------------------------------------------------------------------------------------

  bool transparent() const
  {
    return m_blendMode == BlendMode::Transparent;
  }

  //------------------------------------------------------------------------------------------------

  void setRed(const uint8_t red_)
  {
    m_red = red_;
  }

  //------------------------------------------------------------------------------------------------

  void setGreen(const uint8_t green_)
  {
    m_green = green_;
  }

  //------------------------------------------------------------------------------------------------

  void setBlue(const uint8_t blue_)
  {
    m_blue = blue_;
  }

  //------------------------------------------------------------------------------------------------

  void setAlpha(const uint8_t alpha_)
  {
    m_alpha = alpha_;
  }

  //------------------------------------------------------------------------------------------------

  void setBlendMode(const BlendMode blendMode_)
  {
    m_blendMode = blendMode_;
  }

  //------------------------------------------------------------------------------------------------

  void black()
  {
    m_alpha = 0xFF;
    m_red = m_blue = m_green = m_mono = 0;
    m_blendMode = BlendMode::Normal;
  }

  //------------------------------------------------------------------------------------------------

  void white()
  {
    m_red = m_blue = m_green = m_alpha = m_mono = 0xFF;
    m_blendMode = BlendMode::Normal;
  }

  //------------------------------------------------------------------------------------------------

  void invert()
  {
    m_blendMode = BlendMode::Invert;
  }


  //------------------------------------------------------------------------------------------------

  bool operator==(const Color& other_) const
  {
    return (m_red == other_.m_red) && (m_green == other_.m_green) && (m_blue == other_.m_blue)
           && (m_alpha == other_.m_alpha) && (m_blendMode == other_.m_blendMode);
  }

  //------------------------------------------------------------------------------------------------

  bool operator!=(const Color& other_) const
  {
    return !(operator==(other_));
  }

  //------------------------------------------------------------------------------------------------

  bool operator<(const Color& other_) const
  {
    return getValue() < other_.getValue() && m_alpha < other_.m_alpha
           && m_blendMode <= other_.m_blendMode;
  }

  //------------------------------------------------------------------------------------------------

  bool operator<=(const Color& other_) const
  {
    return getValue() <= other_.getValue() && m_alpha <= other_.m_alpha
           && m_blendMode <= other_.m_blendMode;
  }

  //------------------------------------------------------------------------------------------------

  bool operator>(const Color& other_) const
  {
    return getValue() > other_.getValue() && m_alpha > other_.m_alpha
           && m_blendMode >= other_.m_blendMode;
  }

  //------------------------------------------------------------------------------------------------

  bool operator>=(const Color& other_) const
  {
    return getValue() >= other_.getValue() && m_alpha >= other_.m_alpha
           && m_blendMode >= other_.m_blendMode;
  }

  //------------------------------------------------------------------------------------------------

  uint32_t getValue() const
  {
    return (static_cast<uint32_t>(m_red) << 16) | (static_cast<uint32_t>(m_green) << 8) | m_blue;
  }

  //------------------------------------------------------------------------------------------------

private:
  uint8_t m_red{0U};
  uint8_t m_green{0U};
  uint8_t m_blue{0U};
  uint8_t m_alpha{255U};
  uint8_t m_mono{0U};
  BlendMode m_blendMode{BlendMode::Normal};
};

// -------------------------------------------------------------------------------------------------

} // namespace rastr
} // namespace sl
