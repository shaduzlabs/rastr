/*
        ##########    Copyright (C) 2018 Vincenzo Pacella
        ##      ##    Distributed under MIT license, see file LICENSE
        ##      ##    or <http://opensource.org/licenses/MIT>
        ##      ##
##########      ############################################################# shaduzlabs.com #####*/

#pragma once

#include <rastr/Canvas.hpp>

namespace sl
{
namespace rastr
{

//--------------------------------------------------------------------------------------------------

/**
  \class StaticCanvas
  \brief The canvas base class

*/

class DynamicCanvas : public Canvas
{

public:
  DynamicCanvas(unsigned w_, unsigned h_, unsigned size_ = 0)
    : m_width(w_), m_height(h_), m_size(size_ == 0 ? w_ * h_ * 3 : size_)
  {
    if (m_size > 0)
    {
      m_data = new uint8_t[m_size];
    }
    black();
  }

  ~DynamicCanvas() override
  {
    if (m_data != nullptr)
    {
      delete[] m_data;
    }
  }

  unsigned width() const noexcept override
  {
    return m_width;
  }

  unsigned height() const noexcept override
  {
    return m_height;
  }

  unsigned canvasWidthInBytes() const noexcept override
  {
    return m_height > 0 ? m_size / m_height : 0;
  }

  unsigned bufferSize() const override
  {
    return m_size;
  }

  const uint8_t* data() const override
  {
    return m_data;
  }

protected:
  uint8_t* data() override
  {
    return m_data;
  }

  void resize(unsigned width_, unsigned height_, unsigned size_)
  {
    m_width = width_;
    m_height = height_;
    m_size = size_;
    if (m_data != nullptr)
    {
      delete[] m_data;
    }
    if (size_ > 0)
    {
      m_data = new uint8_t[size_];
    }
  }

private:
  unsigned m_width{0U};
  unsigned m_height{0U};
  unsigned m_size{0U};

  uint8_t* m_data{nullptr}; //!< The raw Canvas data
};

//--------------------------------------------------------------------------------------------------

} // namespace rastr
} // namespace sl
