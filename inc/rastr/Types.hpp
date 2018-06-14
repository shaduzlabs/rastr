/*
        ##########    Copyright (C) 2018 Vincenzo Pacella
        ##      ##    Distributed under MIT license, see file LICENSE
        ##      ##    or <http://opensource.org/licenses/MIT>
        ##      ##
##########      ############################################################# shaduzlabs.com #####*/

#pragma once

#include <stdint.h>
#include <stdlib.h>

namespace sl
{
namespace rastr
{

//--------------------------------------------------------------------------------------------------

//! The alignment type
enum class Alignment
{
  Left,   //!< Align to the left
  Center, //!< Center
  Right,  //!< Align to the right
};

//--------------------------------------------------------------------------------------------------

//! The blend mode
enum class BlendMode
{
  Normal,      // Normal mode
  Invert,      // Invert the current color
  Transparent, // Ignore the current color
};

//--------------------------------------------------------------------------------------------------

struct Bitmap
{
  uint16_t width;
  uint16_t height;
  uint8_t bitsPerPixel;
  const uint8_t* data;
};

//--------------------------------------------------------------------------------------------------

struct Character
{
  uint8_t width;
  uint8_t height;
  int8_t xOffset;
  int8_t yOffset;
  uint8_t xAdvance;
  const uint8_t* data;
};

//--------------------------------------------------------------------------------------------------

class FontId
{
public:
  FontId(const char* name_, unsigned size_) : m_name(name_), m_size(size_)
  {
  }

  const char* name() const
  {
    return m_name;
  }
  unsigned size() const
  {
    return m_size;
  }

  bool operator<(const FontId& id) const
  {
    if (m_size < id.m_size)
    {
      return true;
    }
    return m_name < id.m_name;
  }

private:
  const char* m_name;
  unsigned m_size;
};

//--------------------------------------------------------------------------------------------------

struct Font
{
  FontId id;
  uint8_t bitsPerPixel;
  uint8_t lineHeight;
  uint8_t firstCharacter;
  uint8_t lastCharacter;

  const Character* characters;
};

//--------------------------------------------------------------------------------------------------

class Rectangle{
public:

  Rectangle() = default;

  Rectangle(int x_, int y_, unsigned width_, unsigned height_)
  : m_x(x_)
  , m_y(y_)
  , m_width(width_)
  , m_height(height_)
  {
  }

  int x() const noexcept { return m_x; }
  void setX(int x_) noexcept { m_x = x_; }

  int y() const noexcept { return m_y; }
  void setY(int y_) noexcept { m_y = y_; }

  unsigned width() const noexcept { return m_width; }
  void setWidth(unsigned width_) noexcept { m_width = width_; }

  unsigned height() const noexcept { return m_height; }
  void setHeight(unsigned height_) noexcept { m_height = height_; }

  void set(int x_, int y_, unsigned width_, unsigned height_) noexcept
  {
    m_x = x_;
    m_y = y_;
    m_width = width_;
    m_height = height_;
  }

  void grow(int x_, int y_, unsigned width_, unsigned height_) noexcept
  {
    m_x = abs(x_) > abs(m_x) ? x_ : m_x;
    m_y = abs(y_) > abs(m_y) ? y_ : m_y;
    m_width = width_ > m_width ? width_ : m_width;
    m_height = height_ > m_height ? height_ : m_height;
  }

  void grow(int x_, int y_) noexcept
  {
    if(x_ < m_x)
    {
      auto deltaX = m_x - x_;
      m_x = x_;
      m_width += deltaX;
    }
    else if(x_ > static_cast<int>(m_x + m_width))
    {
      auto deltaX = x_ - m_width - m_x;
      m_width += deltaX;
    }
    if(y_ < m_y)
    {
      auto deltaY = m_y - y_;
      m_y = y_;
      m_height += deltaY;
    }
    else if(y_ > static_cast<int>(m_y + m_height))
    {
      auto deltaY = y_ - m_height - m_y;
      m_height += deltaY;
    }
  }

  void reset() noexcept
  {
    m_x = 0;
    m_y = 0;
    m_width = 0U;
    m_height = 0U;
  }

  operator bool() const noexcept { return m_width > 0 && m_height > 0; }

private:
  int m_x{0};
  int m_y{0};
  unsigned m_width{0U};
  unsigned m_height{0U};
};

} // namespace rastr
} // namespace sl
