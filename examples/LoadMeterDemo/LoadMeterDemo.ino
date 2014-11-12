//
// Load Meter Demo
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
    const uint32_t frame = meg.frameSyncShowLoad();
    
    meg.clearPixels();
    
    if ((frame & B01000000) == 0) {
        meg.setPixel(frame&0x7, (frame>>3)&0x7, Color::red());
    } else {
        meg.fillRect(0, 0, 8, 8, Color(0, frame&0x7, (frame>>3)&0x7));
    }
    
    // ONLY FOR DEMONSTRATION!
    // This is creating an increasing load, which is displayed using the
    // extra LEDs because we are using the method "frameSyncShowLoad()".
    delay((frame>>3)&0xFF);
}