#pragma once
//
// Lucky Resistor's MeggyJr Color
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


#include "Arduino.h"


namespace lr {


// Forward declaration.
class MeggyJr;


/// This represents a RGB color
///
class Color
{
    friend class MeggyJr;
    
public:
    /// Create a new color.
    ///
    /// @param red The red part of the color (0-15).
    /// @param green The green part of the color (0-15).
    /// @param blue The blue part of the color (0-15).
    ///
    Color(uint8_t red, uint8_t green, uint8_t blue);
    
    /// Get the red component of the color
    ///
    /// @return The level of the color from 0-15.
    ///
    uint8_t getRed() const;
    
    /// Get the green component of the color
    ///
    /// @return The level of the color from 0-15.
    ///
    uint8_t getGreen() const;
    
    /// Get the blue component of the color
    ///
    /// @return The level of the color from 0-15.
    ///
    uint8_t getBlue() const;
    
public: // Predefined colors.
    static Color black();
    static Color red();
    static Color orange();
    static Color yellow();
    static Color green();
    static Color blue();
    static Color violet();
    static Color white();
    
    static Color darkRed();
    static Color darkOrange();
    static Color darkYellow();
    static Color darkGreen();
    static Color darkBlue();
    static Color darkViolet();
    static Color gray();
    
    static Color maximum();
    
private:
    uint16_t _color;
};



}