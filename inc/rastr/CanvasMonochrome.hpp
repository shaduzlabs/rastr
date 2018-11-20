/*
        ##########    Copyright (C) 2018 Vincenzo Pacella
        ##      ##    Distributed under MIT license, see file LICENSE
        ##      ##    or <http://opensource.org/licenses/MIT>
        ##      ##
##########      ############################################################# shaduzlabs.com #####*/

#pragma once

#include <rastr/StaticCanvas.hpp>

namespace sl
{
namespace rastr
{

//--------------------------------------------------------------------------------------------------

template <unsigned W, unsigned H>
class CanvasMonochrome : public StaticCanvas<W, H, ((W * H) / 8)>
{
public:
  //------------------------------------------------------------------------------------------------

  //! Set a pixel
  /*!
     \param x_               The X coordinate of the pixel
     \param y_               The Y coordinate of the pixel
     \param color_           The pixel color (RGB + monochrome)
     \param bSetDirtyChunk_  If TRUE, the dirty flag for the pertaining chunk is set
     */
  void setPixel(int x_, int y_, const Color& color_, bool invalidate_) override
  {
    if (x_ >= this->width() || y_ >= this->height() || color_.transparent())
    {
      return;
    }

    auto oldColor = pixel(x_, y_);

    bool isWhite{color_.active()};
    if (color_.blendMode() == sl::rastr::BlendMode::Invert)
    {
      isWhite = !oldColor.active();
    }
    unsigned byteIndex = (this->width() * (y_ >> 3)) + x_;


    if (isWhite)
    {
      this->data()[byteIndex] |= 0x01 << (y_ & 7);
    }
    else
    {
      this->data()[byteIndex] &= ~(0x01 << (y_ & 7));
    }

    if (invalidate_ && oldColor.active() != isWhite)
    {
      this->invalidate(x_, y_);
    }
  }

  //------------------------------------------------------------------------------------------------

  //! Get the pixel value as an RGB color
  /*!
     \param x_               The X coordinate of the pixel
     \param y_               The Y coordinate of the pixel
     \return                 The color of the selected pixel
     */
  Color pixel(int x_, int y_) const override
  {
    if (x_ >= this->width() || y_ >= this->height())
    {
      return {};
    }

    if (((this->data()[x_ + (this->width() * (y_ >> 3))] >> ((y_)&7)) & 0x01) == 0)
    {
      return sl::rastr::Color{0};
    }

    return sl::rastr::Color{0xff};
  }
};

//--------------------------------------------------------------------------------------------------

} // namespace rastr
} // namespace sl
