//
// Display Predefined Colors
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
    
    meg.setPixel(0, 0, Color::black());
    meg.setPixel(1, 0, Color::red());
    meg.setPixel(2, 0, Color::orange());
    meg.setPixel(3, 0, Color::yellow());
    meg.setPixel(4, 0, Color::green());
    meg.setPixel(5, 0, Color::blue());
    meg.setPixel(6, 0, Color::violet());
    meg.setPixel(7, 0, Color::white());
    
    meg.setPixel(1, 2, Color::darkRed());
    meg.setPixel(2, 2, Color::darkOrange());
    meg.setPixel(3, 2, Color::darkYellow());
    meg.setPixel(4, 2, Color::darkGreen());
    meg.setPixel(5, 2, Color::darkBlue());
    meg.setPixel(6, 2, Color::darkViolet());
    meg.setPixel(7, 2, Color::gray());
    
    meg.setPixel(7, 4, Color::maximum());
}


// The loop code.
void loop() 
{
}