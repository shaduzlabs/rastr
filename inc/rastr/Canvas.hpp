/*
        ##########    Copyright (C) 2018 Vincenzo Pacella
        ##      ##    Distributed under MIT license, see file LICENSE
        ##      ##    or <http://opensource.org/licenses/MIT>
        ##      ##
##########      ############################################################# shaduzlabs.com #####*/

#pragma once

#include <rastr/Color.hpp>
#include <rastr/Types.hpp>

#include <cstring>

namespace sl
{
namespace rastr
{

//--------------------------------------------------------------------------------------------------

struct Font;

//--------------------------------------------------------------------------------------------------

/**
  \class Canvas
  \brief The canvas base class

*/

class Canvas
{

public:
  /**
   * @defgroup Canvas Canvas
   * @{
   */

  //--------------------------------------------------------------------------------------------------

  /**
   * @defgroup Types Public types
   * @ingroup Canvas
   * @{
   */

  //! The circle types
  enum class CircleType
  {
    Full,               //!< Full circle
    SemiLeft,           //!< Semicircle (left half)
    SemiTop,            //!< Semicircle (top half)
    SemiRight,          //!< Semicircle (right half)
    SemiBottom,         //!< Semicircle (bottom half)
    QuarterTopLeft,     //!<  Quarter of circle (top left)
    QuarterTopRight,    //!< Quarter of circle (top right)
    QuarterBottomRight, //!< Quarter of circle (bottom right)
    QuarterBottomLeft,  //!< Quarter of circle (bottom left)
  };

  /** @} */ // End of group Types

  //--------------------------------------------------------------------------------------------------

  /**
   * @defgroup Lifetime Constructor and destructor
   * @ingroup Canvas
   * @{
   */

  //------------------------------------------------------------------------------------------------

  //! Destructor
  virtual ~Canvas() = default;

  //------------------------------------------------------------------------------------------------

  //! Get the Canvas width in pixels
  /*!
   \return         The Canvas width in pixels
   */
  virtual unsigned width() const = 0;

  //------------------------------------------------------------------------------------------------

  //! Get the Canvas height in pixels
  /*!
   \return         The Canvas height in pixels
   */
  virtual unsigned height() const = 0;

  //------------------------------------------------------------------------------------------------

  //! Check whether a pixel is within the canvas or not
  /*!
   \return         True if the pixel is within the canvas area
   */
  virtual bool withinCanvas(int x_, int y_) const noexcept
  {
    return x_ >= 0 && y_ >= 0 && static_cast<unsigned>(x_) < width()
           && static_cast<unsigned>(y_) < height();
  }
  //------------------------------------------------------------------------------------------------

  //! Intersect
  /*!
   \return         The portion of the rectangle that is within the canvas
   */
  virtual Rectangle visibleArea(int x_, int y_, unsigned w_, unsigned h_) const noexcept
  {
    int x = (x_ >= 0 ? x_ : 0);
    int y = (y_ >= 0 ? y_ : 0);
    int w = w_ + (x_ >= 0 ? 0 : x_);
    int h = h_ + (y_ >= 0 ? 0 : y_);

    if (w > 0 && h > 0 && withinCanvas(x, y))
    {
      return {x, y, static_cast<unsigned>(w), static_cast<unsigned>(h)};
    }
    return {};
  }

  //------------------------------------------------------------------------------------------------

  //! Get the Canvas width in bytes
  /*!
   \return         The Canvas height in bytes
   */
  virtual unsigned canvasWidthInBytes() const = 0;

  //------------------------------------------------------------------------------------------------

  //! Get the update rectangle
  /*!
   \return         The update rectangle
   */
  virtual const Rectangle& invalidRegion() const noexcept
  {
    return m_invalidRegion;
  }

  //------------------------------------------------------------------------------------------------

  //! Invalidate the whole canvas
  virtual void invalidate() noexcept
  {
    m_invalidRegion.set(0, 0, width(), height());
  }

  //------------------------------------------------------------------------------------------------

  virtual void invalidate(int x_, int y_) noexcept
  {
    m_invalidRegion.grow(x_, y_);
  }

  //------------------------------------------------------------------------------------------------

  //! Add a rectangle to the current invalid region
  virtual void invalidate(int x_, int y_, unsigned w_, unsigned h_) noexcept
  {
    m_invalidRegion.grow(x_, y_, w_, h_);
  }

  //------------------------------------------------------------------------------------------------

  //! Add a rectangle to the current invalid region
  virtual void invalidate(const Rectangle& area_) noexcept
  {
    m_invalidRegion.grow(area_.x(), area_.y(), area_.width(), area_.height());
  }

  //------------------------------------------------------------------------------------------------

  //! Reset the invalid region size
  virtual void resetInvalidRegion() noexcept
  {
    m_invalidRegion.reset();
  }

  /** @} */ // End of group Lifetime

  //--------------------------------------------------------------------------------------------------

  /**
   * @defgroup Fill Fill functions
   * @ingroup Drawing
   * @{
   */

  //------------------------------------------------------------------------------------------------

  //! Fill the Canvas with white pixels
  virtual void white()
  {
    memset(data(), 0xff, bufferSize());
    invalidate();
  }

  //------------------------------------------------------------------------------------------------

  //! Fill the Canvas with black pixels
  virtual void black()
  {
    memset(data(), 0x00, bufferSize());
    invalidate();
  }

  //------------------------------------------------------------------------------------------------

  //! Invert the content of the Canvas
  virtual void invert()
  {
    auto* buffer = data();
    for (unsigned i = 0; i < bufferSize(); ++i)
    {
      buffer[i] = ~buffer[i];
    }
  }

  /** @} */ // End of group Fill

  //------------------------------------------------------------------------------------------------

  /**
   * @defgroup Primitives Drawing primitives
   * @ingroup Drawing
   * @{
   */

  //------------------------------------------------------------------------------------------------

  //! Set a pixel
  /*!
   \param x_               The X coordinate of the pixel
   \param y_               The Y coordinate of the pixel
   \param color_           The pixel color
   \param invalidate_      Add the current pixel to the invalid region
   */
  virtual void setPixel(int x_, int y_, const Color& color_, bool invalidate_ = true)
  {
    if (!withinCanvas(x_, y_) || color_.transparent())
    {
      return;
    }

    Color oldColor = pixel(x_, y_);
    Color newColor = color_;
    unsigned byteIndex = (canvasWidthInBytes() * y_) + (3 * x_);

    if (color_.blendMode() == BlendMode::invert)
    {
      newColor = oldColor;
      newColor.invert();
    }

    data()[byteIndex] = newColor.red();
    data()[byteIndex + 1] = newColor.green();
    data()[byteIndex + 2] = newColor.blue();

    if (invalidate_ && oldColor != newColor)
    {
      invalidate(x_, y_);
    }
  }

  //------------------------------------------------------------------------------------------------

  //! Get the pixel color
  /*!
   \param x_               The X coordinate of the pixel
   \param y_               The Y coordinate of the pixel
   \return                 The color of the selected pixel
   */
  virtual Color pixel(int x_, int y_) const
  {
    if (!withinCanvas(x_, y_))
    {
      return {};
    }
    unsigned byteIndex = (canvasWidthInBytes() * y_) + (3 * x_);

    return {data()[byteIndex], data()[byteIndex + 1], data()[byteIndex + 2]};
  }

  //------------------------------------------------------------------------------------------------

  //! Draw a line
  /*!
   \param x0_              The X coordinate of the first point
   \param y0_              The Y coordinate of the first point
   \param x1_              The X coordinate of the second point
   \param y1_              The Y coordinate of the second point
   \param color_           The line color
   */
  virtual void line(int x0_, int y0_, int x1_, int y1_, const Color& color_)
  {
    // Bresenham line drawing algorithm
    // Original version: Dmitry V. Sokolov

    const auto steep = abs(x0_ - x1_) < abs(y0_ - y1_);
    if (steep)
    {
      util::swap(x0_, y0_);
      util::swap(x1_, y1_);
    }
    if (x0_ > x1_)
    {
      util::swap(x0_, x1_);
      util::swap(y0_, y1_);
    }
    auto dx = x1_ - x0_;
    auto dy = y1_ - y0_;
    auto derror2 = abs(dy) * 2;
    auto y = y0_;
    auto signY = (y1_ > y0_ ? 1 : -1);
    int error2 = 0;
    for (auto x = x0_; x <= x1_; ++x)
    {
      if (steep)
      {
        setPixel(y, x, color_, false);
      }
      else
      {
        setPixel(x, y, color_, false);
      }
      error2 += derror2;
      if (error2 > dx)
      {
        y += signY;
        error2 -= dx * 2;
      }
    }

    invalidate(x0_, y0_);
    invalidate(x1_, y1_);
  }

  //------------------------------------------------------------------------------------------------

  //! Draw a vertical line
  /*!
   \param x_               The X coordinate of the starting point
   \param y_               The Y coordinate of the starting point
   \param l_               The line length
   \param color_           The line color
   */
  virtual void lineVertical(int x_, int y_, unsigned l_, const Color& color_)
  {
    for (int y = y_; y < y_ + static_cast<int>(l_); ++y)
    {
      setPixel(x_, y, color_, false);
    }

    invalidate(x_, y_, 1, l_);
  }

  //------------------------------------------------------------------------------------------------

  //! Draw an horizontal line
  /*!
   \param x_               The X coordinate of the starting point
   \param y_               The Y coordinate of the starting point
   \param l_               The line length
   \param color_           The line color
   */
  virtual void lineHorizontal(int x_, int y_, unsigned l_, const Color& color_)
  {
    for (auto x = x_; x < x_ + static_cast<int>(l_); ++x)
    {
      setPixel(x, y_, color_, false);
    }
    invalidate(x_, y_, l_, 1);
  }

  //------------------------------------------------------------------------------------------------

  //! Draw a triangle
  /*!
   \param x0_              The X coordinate of the first point
   \param y0_              The Y coordinate of the first point
   \param x1_              The X coordinate of the second point
   \param y1_              The Y coordinate of the second point
   \param x2_              The X coordinate of the third point
   \param y2_              The Y coordinate of the third point
   \param color_           The line color
  */
  virtual void triangle(int x0_, int y0_, int x1_, int y1_, int x2_, int y2_, const Color& color_)
  {
    line(x0_, y0_, x1_, y1_, color_);
    line(x1_, y1_, x2_, y2_, color_);
    line(x2_, y2_, x0_, y0_, color_);
  }

  //------------------------------------------------------------------------------------------------

  //! Draw a filled triangle
  /*!
   \param x0_              The X coordinate of the first point
   \param y0_              The Y coordinate of the first point
   \param x1_              The X coordinate of the second point
   \param y1_              The Y coordinate of the second point
   \param x2_              The X coordinate of the third point
   \param y2_              The Y coordinate of the third point
   \param color_           The border color
   \param fillColor_       The fill color
  */
  virtual void triangleFilled(int x0_,
    int y0_,
    int x1_,
    int y1_,
    int x2_,
    int y2_,
    const Color& color_,
    const Color& fillColor_)
  {

    auto flatSideTriangle = [this, x0_, y0_, x1_, y1_, x2_, y2_, fillColor_](
                              int x0, int y0, int x1, int y1, int x2, int y2, const Color& color) {
      int tmpX1 = x0;
      int tmpY1 = y0;
      int tmpX2 = x0;
      int tmpY2 = y0;

      bool changed1 = false;
      bool changed2 = false;

      int dx1 = abs(x1 - x0);
      int dy1 = abs(y1 - y0);

      int dx2 = abs(x2 - x0);
      int dy2 = abs(y2 - y0);

      int signx1 = (x1 - x0) >= 0 ? 1 : -1;
      int signx2 = (x2 - x0) >= 0 ? 1 : -1;

      int signy1 = (y1 - y0) >= 0 ? 1 : -1;
      int signy2 = (y2 - y0) >= 0 ? 1 : -1;

      if (dy1 > dx1)
      {
        util::swap(dx1, dy1);
        changed1 = true;
      }

      if (dy2 > dx2)
      {
        util::swap(dx2, dy2);
        changed2 = true;
      }

      int e1 = 2 * dy1 - dx1;
      int e2 = 2 * dy2 - dx2;

      int savedX = -1;
      int savedY = -1;
      int savedL = -1;

      for (int i = 0; i <= dx1; i++)
      {
        while (e1 >= 0)
        {
          if (changed1)
          {
            tmpX1 += signx1;
          }
          else
          {
            tmpY1 += signy1;
          }
          e1 = e1 - 2 * dx1;
        }

        if (changed1)
        {
          tmpY1 += signy1;
        }
        else
        {
          tmpX1 += signx1;
        }
        e1 = e1 + 2 * dy1;

        while (tmpY2 != tmpY1)
        {
          while (e2 >= 0)
          {
            if (changed2)
            {
              tmpX2 += signx2;
            }
            else
            {
              tmpY2 += signy2;
            }
            e2 = e2 - 2 * dx2;
          }

          if (changed2)
          {
            tmpY2 += signy2;
          }
          else
          {
            tmpX2 += signx2;
          }
          e2 = e2 + 2 * dy2;
        }

        auto length = abs(tmpX2 - tmpX1);
        auto xCoord = util::min(tmpX1, tmpX2);

        if (((signy1 > 0 && tmpY1 < y1) || (signy1 < 0 && tmpY1 >= y1)))
        {
          if (savedL > 0 && savedY != tmpY1)
          {
            lineHorizontal(savedX, savedY, savedL, color);
          }
          savedX = xCoord;
          savedY = tmpY1;
          savedL = length;
        }
      }

      lineHorizontal(savedX, savedY, savedL, color);
    };


    if (x0_ == x1_ && x1_ == x2_)
    {
      const auto minY = util::min(y0_, y1_, y2_);
      const auto length = util::max(y0_, y1_, y2_) - minY;
      lineVertical(x0_, minY, length, color_);
      return;
    }

    if (y0_ == y1_ && y1_ == y2_)
    {
      const auto minX = util::min(x0_, x1_, x2_);
      const auto length = util::max(x0_, x1_, x2_) - minX;
      lineHorizontal(minX, y0_, length, color_);
      return;
    }

    if (y1_ < y0_)
    {
      util::swap(x0_, x1_);
      util::swap(y0_, y1_);
    }

    if (y2_ < y0_)
    {
      util::swap(x0_, x2_);
      util::swap(y0_, y2_);
    }

    if (y2_ < y1_)
    {
      util::swap(x1_, x2_);
      util::swap(y1_, y2_);
    }

    if (y1_ == y2_)
    {
      flatSideTriangle(x0_, y0_, x1_, y1_, x2_, y2_, fillColor_);
    }
    else if (y0_ == y1_)
    {
      flatSideTriangle(x2_, y2_, x0_, y0_, x1_, y1_, fillColor_);
    }
    else
    {
      auto x4 = x0_ + (int)(((y1_ - y0_) * (x2_ - x0_)) / (float)(y2_ - y0_));
      flatSideTriangle(x0_, y0_, x1_, y1_, x4, y1_, fillColor_);
      flatSideTriangle(x2_, y2_, x1_, y1_, x4, y1_, fillColor_);
    }

    line(x0_, y0_, x1_, y1_, color_);
    line(x1_, y1_, x2_, y2_, color_);
    line(x2_, y2_, x0_, y0_, color_);
  }

  //------------------------------------------------------------------------------------------------

  //! Draw a rectangle
  /*!
   \param x_              The X coordinate of the upper-left corner
   \param y_              The Y coordinate of the upper-left corner
   \param w_              The width of the rectangle
   \param h_              The height of the rectangle
   \param color_          The line color
  */
  virtual void rectangle(int x_, int y_, unsigned w_, unsigned h_, const Color& color_)
  {
    rectangleFilled(x_, y_, w_, h_, color_, {});
  }

  //------------------------------------------------------------------------------------------------

  //! Draw a filled rectangle
  /*!
   \param x_              The X coordinate of the upper-left corner
   \param y_              The Y coordinate of the upper-left corner
   \param w_              The width of the rectangle
   \param h_              The height of the rectangle
   \param color_          The border color
   \param fillColor_      The fill color
  */
  virtual void rectangleFilled(
    int x_, int y_, unsigned w_, unsigned h_, const Color& color_, const Color& fillColor_)
  {
    if (x_ > static_cast<int>(width()) || y_ > static_cast<int>(height()) || w_ == 0u || h_ == 0u)
    {
      return;
    }

    lineHorizontal(x_, y_, w_, color_);
    lineHorizontal(x_, y_ + h_ - 1, w_, color_);
    if (h_ <= 2)
    {
      return;
    }

    lineVertical(x_, y_ + 1, h_ - 2, color_);
    lineVertical(x_ + w_ - 1, y_ + 1, h_ - 2, color_);

    if (fillColor_.transparent())
    {
      return;
    }

    if (w_ > h_)
    {
      unsigned lineWidth = w_ - 2;
      for (unsigned i = y_ + 1; i < y_ + h_ - 1; ++i)
      {
        lineHorizontal(x_ + 1, i, lineWidth, fillColor_);
      }
    }
    else
    {
      unsigned lineHeight = h_ - 2;
      for (unsigned i = x_ + 1; i < x_ + w_ - 1; ++i)
      {
        lineVertical(i, y_ + 1, lineHeight, fillColor_);
      }
    }
  }

  //------------------------------------------------------------------------------------------------

  //! Draw a rectangle with rounded corners
  /*!
   \param x_              The X coordinate of the upper-left corner
   \param y_              The Y coordinate of the upper-left corner
   \param w_              The width of the rectangle
   \param h_              The height of the rectangle
   \param r_              The corner radius
   \param color_          The line color
  */
  virtual void rectangleRounded(
    int x_, int y_, unsigned w_, unsigned h_, unsigned r_, const Color& color_)
  {
    rectangleRoundedFilled(x_, y_, w_, h_, r_, color_, {});
  }

  //------------------------------------------------------------------------------------------------

  //! Draw a filled rectangle with rounded corners
  /*!
   \param x_              The X coordinate of the upper-left corner
   \param y_              The Y coordinate of the upper-left corner
   \param w_              The width of the rectangle
   \param h_              The height of the rectangle
   \param r_              The corner radius
   \param color_          The border color
   \param fillColor_      The fill color
  */
  virtual void rectangleRoundedFilled(int x_,
    int y_,
    unsigned w_,
    unsigned h_,
    unsigned r_,
    const Color& color_,
    const Color& fillColor_)
  {
    if (x_ > static_cast<int>(width()) || y_ > static_cast<int>(height()) || w_ == 0u || h_ == 0u)
    {
      return;
    }

    unsigned smallestSide = (w_ >= h_) ? h_ : w_;
    unsigned rOffset = 2 * r_;
    if (rOffset > smallestSide)
    {
      r_ = smallestSide / 2;
      rOffset = smallestSide;
    }
    lineHorizontal((x_ + r_), (y_), (w_ - rOffset), color_);
    lineHorizontal((x_ + r_), (y_ + h_ - 1), (w_ - rOffset), color_);
    lineVertical((x_), (y_ + r_), (h_ - rOffset), color_);
    lineVertical((x_ + w_ - 1), (y_ + r_), (h_ - rOffset), color_);

    circleFilled((x_ + r_), (y_ + r_), r_, color_, fillColor_, CircleType::QuarterTopLeft);

    circleFilled(
      (x_ + w_ - r_ - 1), (y_ + r_), r_, color_, fillColor_, CircleType::QuarterTopRight);

    circleFilled((x_ + w_ - r_ - 1),
      (y_ + h_ - r_ - 1),
      r_,
      color_,
      fillColor_,
      CircleType::QuarterBottomRight);

    circleFilled(
      (x_ + r_), (y_ + h_ - r_ - 1), r_, color_, fillColor_, CircleType::QuarterBottomLeft);

    if (fillColor_.transparent() || h_ <= 2 || w_ <= 2)
    {
      return;
    }

    rectangleFilled((x_ + r_), (y_ + 1), (w_ - rOffset), r_, fillColor_, fillColor_);

    rectangleFilled((x_ + 1), (y_ + r_), (w_ - 2), (h_ - rOffset), fillColor_, fillColor_);

    rectangleFilled((x_ + r_), (y_ + h_ - 1 - r_), (w_ - rOffset), (r_), fillColor_, fillColor_);
  }

  //------------------------------------------------------------------------------------------------

  //! Draw a circle
  /*!
   \param x_              The X coordinate of the center
   \param y_              The Y coordinate of the center
   \param r_              The radius
   \param color_          The line color
   \param type_           The circle type
  */
  virtual void circle(
    int x_, int y_, unsigned r_, const Color& color_, CircleType type_ = CircleType::Full)
  {
    circleFilled(x_, y_, r_, color_, {}, type_);
  }

  //------------------------------------------------------------------------------------------------

  //! Draw a filled circle
  /*!
   \param x_              The X coordinate of the center
   \param y_              The Y coordinate of the center
   \param r_              The radius
   \param color_          The border color
   \param fillColor_      The fill color
   \param type_           The circle type
  */
  virtual void circleFilled(int x_,
    int y_,
    unsigned r_,
    const Color& color_,
    const Color& fillColor_,
    CircleType type_ = CircleType::Full)
  {
    if (r_ == 0)
    {
      return;
    }

    const auto diameter = r_ * 2;
    int x, y, rX0, rX1, rY0, rY1;
    rX0 = rY0 = -1 * static_cast<int>(r_);
    rX1 = rY1 = r_;
    int rsq = r_ * r_;

    switch (type_)
    {
      case CircleType::SemiLeft:
        rX1 = 0;
        break;
      case CircleType::SemiRight:
        rX0 = 0;
        break;
      case CircleType::SemiTop:
        rY1 = 0;
        break;
      case CircleType::SemiBottom:
        rY0 = 0;
        break;

      case CircleType::QuarterTopLeft:
        rX1 = 0;
        rY1 = 0;
        break;
      case CircleType::QuarterTopRight:
        rX0 = 0;
        rY1 = 0;
        break;
      case CircleType::QuarterBottomLeft:
        rX1 = 0;
        rY0 = 0;
        break;
      case CircleType::QuarterBottomRight:
        rX0 = 0;
        rY0 = 0;
        break;

      default:
      case CircleType::Full:
        break;
    }

    for (x = rX0; x <= rX1; ++x)
    {
      const int xAbs = x + x_;
      if (xAbs < 0 || xAbs >= static_cast<int>(width()))
        continue;
      for (y = rY0; y <= rY1; ++y)
      {
        const int yAbs = y + y_;
        if (yAbs < 0 || yAbs >= static_cast<int>(height()))
          continue;
        int xysq = ((x * x) + (y * y));
        if ((rsq - xysq < static_cast<int>(r_)) && (xysq - rsq < static_cast<int>(r_)))
        {
          setPixel(xAbs, yAbs, color_, false);
        }
        else if (xysq < rsq)
        {
          setPixel(xAbs, yAbs, fillColor_, false);
        }
      }
    }

    invalidate(x_ - r_, y_ - r_, diameter, diameter);
  }

  //------------------------------------------------------------------------------------------------

  //! Place a bitmap on the canvas
  /*!
   \param x_              The X coordinate of the upper-left corner
   \param y_              The Y coordinate of the upper-left corner
   \param w_              The bitmap width
   \param h_              The bitmap height
   \param bitmap_         The raw bitmap data
   \param bitsPerPixel_   The number of bits per pixel
   \param color_          The drawing color
  */
  virtual Rectangle putBitmap(const int x_,
    const int y_,
    const unsigned w_,
    const unsigned h_,
    const uint8_t* bitmap_,
    const uint8_t bitsPerPixel_,
    const Color& color_)
  {
    const auto area = visibleArea(x_, y_, w_, h_);
    if (!area || (bitsPerPixel_ != 1 && bitsPerPixel_ != 8))
    {
      return {};
    }

    const auto bitmapOffsetX = area.x() - x_;
    const auto bitmapOffsetY = area.y() - y_;

    unsigned pixelIndex = (w_ * bitmapOffsetY) + bitmapOffsetX;
    for (unsigned j = 0; j < area.height(); ++j)
    {
      for (unsigned i = 0; i < area.width(); ++i)
      {
        if (bitsPerPixel_ == 1)
        {
          if (bitmap_[pixelIndex >> 3] & (0x01 << (7 - (pixelIndex & 7))))
          {
            setPixel(x_ + i, y_ + j, color_, false);
          }
        }
        else if (bitsPerPixel_ == 8 && bitmap_[pixelIndex] > 0)
        {
          setPixel(x_ + i, y_ + j, {color_, bitmap_[pixelIndex]}, false);
        }
        ++pixelIndex;
      }

      pixelIndex += w_ - area.width();
    }
    invalidate(area);

    return area;
  }

  //------------------------------------------------------------------------------------------------

  virtual Rectangle putBitmap(int x_, int y_, const Bitmap& bitmap_, const Color& color_)
  {
    return putBitmap(
      x_, y_, bitmap_.width, bitmap_.height, bitmap_.data, bitmap_.bitsPerPixel, color_);
  }

  //------------------------------------------------------------------------------------------------

  virtual void putCanvas(const Canvas& c_,
    int xDest_,
    int yDest_,
    int xSource_ = 0,
    int ySource_ = 0,
    unsigned w_ = 0,
    unsigned h_ = 0)
  {
    unsigned cw = c_.width();
    unsigned ch = c_.height();

    if ((xDest_ >= static_cast<int>(width())) || (yDest_ >= static_cast<int>(height()))
        || (xSource_ >= static_cast<int>(cw)) || (ySource_ >= static_cast<int>(ch)))
    {
      return;
    }

    unsigned ww = (w_ <= cw && w_ > 0) ? w_ : cw;
    unsigned hh = (h_ <= ch && h_ > 0) ? h_ : ch;

    unsigned drawableHeight = ((yDest_ + hh) > height()) ? (height() - yDest_) : hh;
    unsigned drawableWidth = ((xDest_ + ww) > width()) ? (width() - xDest_) : ww;

    for (unsigned j = 0; j < drawableHeight; ++j)
    {
      for (unsigned i = 0; i < drawableWidth; ++i)
      {
        setPixel(xDest_ + i, yDest_ + j, c_.pixel(xSource_ + i, ySource_ + j), false);
      }
    }
    invalidate(xDest_, yDest_, drawableWidth, drawableHeight);
  }

  /** @} */ // End of group Primitives

  //------------------------------------------------------------------------------------------------

  /**
   * @defgroup Text Text
   * @ingroup Drawing
   * @{
   */

  //------------------------------------------------------------------------------------------------

  //! Place a single character on the canvas
  /*!
   \param x_             The X coordinate
   \param y_             The Y coordinate
   \param c_             The char to be printed
   \param font_          The font
   \param color_         The color of the pixels
   */
  virtual uint8_t putCharacter(int x_, int y_, char c_, const Color& color_, const Font& font_)
  {
    if (c_ > font_.lastCharacter || c_ < font_.firstCharacter)
    {
      return 0;
    }

    const auto& character = font_.characters[c_ - font_.firstCharacter];
    if ((x_ >= static_cast<int>(width()))
        || ((y_ - character.yOffset + font_.lineHeight) >= static_cast<int>(height())))
    {
      return 0;
    }

    const auto x = x_ + character.xOffset;
    const auto y = y_ - character.yOffset;
    const auto area = putBitmap(
      x, y, character.width, character.height, character.data, font_.bitsPerPixel, color_);

    return x >= 0 ? character.xAdvance : character.xAdvance - (area.x() - x);
  }

  //------------------------------------------------------------------------------------------------

  //! Place a text string on the canvas
  /*!
   \param x_             The X coordinate
   \param y_             The Y coordinate
   \param text_          The string to be printed
   \param color_         The color of the pixels
   \param font_          The font
   \param spacing_       Additional space between the characters in pixels
   */
  virtual void putText(
    int x_, int y_, const char* text_, const Color& color_, const Font& font_, unsigned spacing_ = 0)
  {
    uint8_t charWidth = spacing_;
    const int lineHeight = static_cast<int>(font_.lineHeight);
    if ((y_ > lineHeight && (y_ - lineHeight) >= static_cast<int>(height()))
        || x_ > static_cast<int>(width()))
    {
      return;
    }

    for (unsigned i = 0; static_cast<unsigned>(text_[i]) != 0; ++i)
    {
      if (x_ > static_cast<int>(width() + charWidth))
      {
        return;
      }
      x_ += putCharacter(x_, y_, text_[i], color_, font_) + spacing_;
    }
  }

  /**@}*/ // End of Text group

  //------------------------------------------------------------------------------------------------

  /** @} */ // End of group Drawing

  //------------------------------------------------------------------------------------------------

  /**
   * @defgroup Configuration Configuration of Canvas properties
   * @ingroup Canvas
   * @{
   */

  //------------------------------------------------------------------------------------------------

  /** @} */ // End of group Configuration

  //------------------------------------------------------------------------------------------------

  /**
   * @defgroup Access Access and state queries functions
   * @ingroup Canvas
   * @{
   */

  /** @} */ // End of group Access

  /** @} */ // End of group Canvas

  //------------------------------------------------------------------------------------------------

  virtual unsigned bufferSize() const = 0;

  virtual const uint8_t* data() const = 0;

protected:
  virtual uint8_t* data() = 0;

  Rectangle m_invalidRegion;
};

//--------------------------------------------------------------------------------------------------

} // namespace rastr
} // namespace sl
