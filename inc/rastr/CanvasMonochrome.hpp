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

    Color oldColor = pixel(x_, y_);
    Color newColor = color_;
    if (color_.blendMode() == BlendMode::Invert)
    {
      newColor = oldColor;
      newColor.invert();
    }

    unsigned byteIndex = (this->canvasWidthInBytes() * y_) + x_;

    this->data()[byteIndex] = color_.mono();

    if (invalidate_ && oldColor.mono() != newColor.mono())
    {
      Canvas::invalidate(x_, y_);
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
    unsigned index = (this->canvasWidthInBytes() * y_) + x_;

    return Color{static_cast<uint8_t>(this->data()[index])};
  }
};

//--------------------------------------------------------------------------------------------------

} // namespace rastr
} // namespace sl
