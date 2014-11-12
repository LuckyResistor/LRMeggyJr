//
// Button Handling Demo
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
    meg.setup(MeggyJr::FrameRate15);
}


// The loop code.
void loop()
{
    const uint32_t frame = meg.frameSync();

    meg.scrollPixel(MeggyJr::ScrollLeft);
    for (uint8_t y = 8; y != 0; --y) {
        meg.setPixel(7, y-1, Color::black());
    }
    
    if (meg.isAButtonDown()) {
        meg.setPixel(7, 1, Color::white());
    }
    if (meg.isAButtonPressed()) {
        meg.setPixel(7, 1, Color::red());
    }
    if (meg.isAButtonReleased()) {
        meg.setPixel(7, 1, Color::green());
    }

    if (meg.isBButtonDown()) {
        meg.setPixel(7, 2, Color::white());
    }
    if (meg.isBButtonPressed()) {
        meg.setPixel(7, 2, Color::red());
    }
    if (meg.isBButtonReleased()) {
        meg.setPixel(7, 2, Color::green());
    }

    if (meg.isUpButtonDown()) {
        meg.setPixel(7, 3, Color::white());
    }
    if (meg.isUpButtonPressed()) {
        meg.setPixel(7, 3, Color::red());
    }
    if (meg.isUpButtonReleased()) {
        meg.setPixel(7, 3, Color::green());
    }

    if (meg.isRightButtonDown()) {
        meg.setPixel(7, 4, Color::white());
    }
    if (meg.isRightButtonPressed()) {
        meg.setPixel(7, 4, Color::red());
    }
    if (meg.isRightButtonReleased()) {
        meg.setPixel(7, 4, Color::green());
    }

    if (meg.isDownButtonDown()) {
        meg.setPixel(7, 5, Color::white());
    }
    if (meg.isDownButtonPressed()) {
        meg.setPixel(7, 5, Color::red());
    }
    if (meg.isDownButtonReleased()) {
        meg.setPixel(7, 5, Color::green());
    }

    if (meg.isLeftButtonDown()) {
        meg.setPixel(7, 6, Color::white());
    }
    if (meg.isLeftButtonPressed()) {
        meg.setPixel(7, 6, Color::red());
    }
    if (meg.isLeftButtonReleased()) {
        meg.setPixel(7, 6, Color::green());
    }
    
    if ((frame & B00000100) != 0) {
        meg.setPixel(7, 0, Color::blue());
        meg.setPixel(7, 7, Color::blue());
    }
}
