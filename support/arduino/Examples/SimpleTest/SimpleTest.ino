/*
        ##########    Copyright (C) 2018 Vincenzo Pacella
        ##      ##    Distributed under MIT license, see file LICENSE
        ##      ##    or <http://opensource.org/licenses/MIT>
        ##      ##
##########      ############################################################# shaduzlabs.com #####*/

#include "default_monospace_7.h"

#include <rastr.h>

//--------------------------------------------------------------------------------------------------

sl::rastr::CanvasMonochrome<128,64> canvas;

//--------------------------------------------------------------------------------------------------

void setup(){
  canvas.putText(8, 8, "Hello world!", {0xff}, k_font_DefaultMonospace_7);
}

void loop() {

}
