/*
        ##########    Copyright (C) 2018 Vincenzo Pacella
        ##      ##    Distributed under MIT license, see file LICENSE
        ##      ##    or <http://opensource.org/licenses/MIT>
        ##      ##
##########      ############################################################# shaduzlabs.com #####*/

#pragma once

#include <rastr/DynamicCanvas.hpp>

#include <string>

//--------------------------------------------------------------------------------------------------

#define DO_WRITE_PICTURES 1 // Enable this to generate the pngs, must be disabled on build servers

//--------------------------------------------------------------------------------------------------

namespace sl
{
namespace rastr
{
namespace test
{

//--------------------------------------------------------------------------------------------------

class PngCanvas : public DynamicCanvas
{
public:
  PngCanvas();
  PngCanvas(unsigned w_, unsigned h_, unsigned size_ = 0);

  void read(const std::string& fileName_);
  void write(const std::string& fileName_);
};

//--------------------------------------------------------------------------------------------------

bool matchColorForAllPixels(const Canvas* /*canvas_*/, const Color& /*color_*/);

bool compare(const Canvas* /*c1_*/, const Canvas* /*c2_*/);

//--------------------------------------------------------------------------------------------------

} // namespace test
} // namespace rastr
} // namespace sl
