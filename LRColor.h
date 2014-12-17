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

#include <avr/pgmspace.h>


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
    Color(const uint8_t red, const uint8_t green, const uint8_t blue);
    
    /// Create a color from a given 16bit value.
    ///
    /// @param colorValue The 16 bit value in the format 0x0RGB.
    ///
    Color(const uint16_t colorValue);
    
    /// Create a color from a value in program memory.
    ///
    static inline Color fromProgMem(const uint16_t *colorValuePtr) {
        return Color(pgm_read_word(colorValuePtr));
    }
    
public:
    /// Get the red component of the color
    ///
    /// @return The level of the color from 0-15.
    ///
    inline uint8_t getRed() const { return (_color >> 8) & 0x0f; }
    
    /// Get the green component of the color
    ///
    /// @return The level of the color from 0-15.
    ///
    inline uint8_t getGreen() const { return (_color >> 4) & 0x0f; }
    
    /// Get the blue component of the color
    ///
    /// @return The level of the color from 0-15.
    ///
    inline uint8_t getBlue() const { return (_color & 0x0f); }
    
    /// Get a darker color.
    ///
    /// The value can be between 16 (original color) and 0 (black).
    ///
    inline Color getDarker(const uint8_t shade) {
        return Color(getRed()*shade/16, getGreen()*shade/16, getBlue()*shade/16);
    }
    
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

    
/// Use this macro to define static color values in program memory arrays.
///
/// When defined in SRAM, use the Color() constructor to convert the value into
/// a color object. When defined in PROGMEM, use the static method fromProgMem()
/// to create a color object from it.
///
#define LRCOLOR_STATIC(r, g, b) ((uint16_t)(((r) << 8)|((g) << 4)|(b)))


}


