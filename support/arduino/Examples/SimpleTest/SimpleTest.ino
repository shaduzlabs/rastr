/*
        ##########    Copyright (C) 2018 Vincenzo Pacella
        ##      ##    Distributed under MIT license, see file LICENSE
        ##      ##    or <http://opensource.org/licenses/MIT>
        ##      ##
##########      ############################################################# shaduzlabs.com #####*/

#include <rastr.h>

#include "default_monospace_7.h"

//--------------------------------------------------------------------------------------------------

sl::rastr::CanvasMonochrome<128,64> canvas;

//--------------------------------------------------------------------------------------------------

void setup(){
  canvas.putText(8, 8, "Hello world!", sl::rastr::Color{0xff}, k_font_DefaultMonospace_7);
}

void loop() {

}