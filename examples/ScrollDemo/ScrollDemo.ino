//
// Scroll Demo
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
    
    meg.fillRect(1, 1, 6, 6, Color::red());
    meg.fillRect(2, 2, 4, 4, Color::green());
}

// Use phase to make sure we start at 0
uint16_t phase = 0;

// The loop code.
void loop()
{
    const uint32_t frame = meg.frameSyncShowLoad();
    
    if ((phase & 3) == 0) {
        switch ((phase >> 8) & 3) {
            case 0: meg.scrollPixel(MeggyJr::ScrollUp); break;
            case 1: meg.scrollPixel(MeggyJr::ScrollRight); break;
            case 2: meg.scrollPixel(MeggyJr::ScrollDown); break;
            case 3: meg.scrollPixel(MeggyJr::ScrollLeft); break;
        }
    }
    ++phase;
}
