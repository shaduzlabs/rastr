/*
        ##########    Copyright (C) 2018 Vincenzo Pacella
        ##      ##    Distributed under MIT license, see file LICENSE
        ##      ##    or <http://opensource.org/licenses/MIT>
        ##      ##
##########      ############################################################# shaduzlabs.com #####*/

#pragma once

#include <rastr/Canvas.hpp>

//--------------------------------------------------------------------------------------------------

namespace sl
{
namespace rastr
{

//--------------------------------------------------------------------------------------------------

/**
  \class StaticCanvas
  \brief The canvas base class

*/
template <unsigned W, unsigned H, unsigned S = W* H * 3>
class StaticCanvas : public Canvas
{

public:
  StaticCanvas()
  {
    invalidate();
  }

  unsigned width() const noexcept override
  {
    return W;
  }

  unsigned height() const noexcept override
  {
    return H;
  }

  unsigned canvasWidthInBytes() const noexcept override
  {
    return H > 0 ? S / H : 0;
  }

  unsigned bufferSize() const override
  {
    return S;
  }

  const uint8_t* data() const override
  {
    return &m_data[0];
  }

protected:
  uint8_t* data() override
  {
    return &m_data[0];
  }

  uint8_t m_data[S]; //!< The raw Canvas data
};

//--------------------------------------------------------------------------------------------------

} // namespace rastr
} // namespace sl
