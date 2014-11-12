//
// Use the Extra LEDs
// ---------------------------------------------------------------------------
// (c)2014 by Lucky Resistor. See LICENSE for details.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//


#include <LRMeggyJr.h>


using namespace lr;


// The setup code.
void setup() 
{
    meg.setup();
}


// The loop code.
void loop() 
{
    for (uint8_t i = 0; i != 255; ++i) {
        meg.setExtraLeds(i);
        delay(10);
    }
    
    meg.clear();
    
    for (uint8_t i = 0; i < 8; ++i) {
        meg.enableExtraLed(i);
        delay(200);
    }

    for (uint8_t i = 0; i < 8; ++i) {
        meg.disableExtraLed(i);
        delay(200);
    }
}