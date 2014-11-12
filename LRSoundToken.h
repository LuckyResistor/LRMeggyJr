#pragma once
//
// Lucky Resistor's MeggyJr SoundToken
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

    
/// A single sound token used to define a sound.
///
enum SoundToken : uint8_t {
    
    SoundEnd = 0x00, // End of a sound definition.
    
    NoteA0  = 0x01,  // With 1/8 prescaler (use for low fx)
    NoteAs0 = 0x02,
    NoteH0  = 0x03,
    
    NoteC1  = 0x04,  // Set frequency to C octave 1
    NoteCs1 = 0x05,  // Set frequency to C# octave 1
    NoteD1  = 0x06,  // ...
    NoteDs1 = 0x07,  // ...
    NoteE1  = 0x08,
    NoteF1  = 0x09,
    NoteFs1 = 0x0a,
    NoteG1  = 0x0b,
    NoteGs1 = 0x0c,
    NoteA1  = 0x0d,
    NoteAs1 = 0x0e,
    NoteH1  = 0x0f,
    
    NoteC2  = 0x10,  // Set frequency to C octave 2
    NoteCs2 = 0x11,  // Set frequency to C# octave 2
    NoteD2  = 0x12,  // ...
    NoteDs2 = 0x13,  // ...
    NoteE2  = 0x14,
    NoteF2  = 0x15,
    NoteFs2 = 0x16,
    NoteG2  = 0x17,
    NoteGs2 = 0x18,
    NoteA2  = 0x19,  // Without prescaler from here (use for high fx).
    NoteAs2 = 0x1a,
    NoteH2  = 0x1b,
    
    NoteC3  = 0x1c,  // Set frequency to C octave 3
    NoteCs3 = 0x1d,  // Set frequency to C# octave 3
    NoteD3  = 0x1e,  // ...
    NoteDs3 = 0x1f,  // ...
    NoteE3  = 0x20,
    NoteF3  = 0x21,
    NoteFs3 = 0x22,
    NoteG3  = 0x23,
    NoteGs3 = 0x24,
    NoteA3  = 0x25,
    NoteAs3 = 0x26,
    NoteH3  = 0x27,
    
    NoteC4  = 0x28,  // Set frequency to C octave 4
    NoteCs4 = 0x29,  // Set frequency to C# octave 4
    NoteD4  = 0x2a,  // ...
    NoteDs4 = 0x2b,  // ...
    NoteE4  = 0x2c,
    NoteF4  = 0x2d,
    NoteFs4 = 0x2e,
    NoteG4  = 0x2f,
    NoteGs4 = 0x30,
    NoteA4  = 0x31,  // 440 Hz
    NoteAs4 = 0x32,
    NoteH4  = 0x33,
    
    NoteC5  = 0x34,  // Set frequency to C octave 5
    NoteCs5 = 0x35,  // Set frequency to C# octave 5
    NoteD5  = 0x36,  // ...
    NoteDs5 = 0x37,  // ...
    NoteE5  = 0x38,
    NoteF5  = 0x39,
    NoteFs5 = 0x3a,
    NoteG5  = 0x3b,
    NoteGs5 = 0x3c,
    NoteA5  = 0x3d,
    NoteAs5 = 0x3e,
    NoteH5  = 0x3f,
    
    NoteC6  = 0x40,  // Set frequency to C octave 6
    NoteCs6 = 0x41,  // Set frequency to C# octave 6
    NoteD6  = 0x42,  // ...
    NoteDs6 = 0x43,  // ...
    NoteE6  = 0x44,
    NoteF6  = 0x45,
    NoteFs6 = 0x46,
    NoteG6  = 0x47,
    NoteGs6 = 0x48,
    NoteA6  = 0x49,
    NoteAs6 = 0x4a,
    NoteH6  = 0x4b,
    
    NoteC7  = 0x4c,  // Set frequency to C octave 7
    NoteCs7 = 0x4d,  // Set frequency to C# octave 7
    NoteD7  = 0x4e,  // ...
    NoteDs7 = 0x4f,  // ...
    NoteE7  = 0x50,
    NoteF7  = 0x51,
    NoteFs7 = 0x52,
    NoteG7  = 0x53,
    NoteGs7 = 0x54,
    
    Play1   = 0x80, // Play the note for 1/1
    Play2   = 0x81, // Play the note for 1/2
    Play4   = 0x82, // Play the note for 1/4
    Play8   = 0x83, // Play the note for 1/8
    Play16  = 0x84, // Play the note for 1/16
    Play32  = 0x85, // Play the note for 1/32
    Play64  = 0x86, // Play the note for 1/64
    
    Pause1  = 0xa0, // Pause for 1/1
    Pause2  = 0xa1, // Pause for 1/2
    Pause4  = 0xa2, // Pause for 1/4
    Pause8  = 0xa3, // Pause for 1/8
    Pause16 = 0xa4, // Pause for 1/16
    Pause32 = 0xa5, // Pause for 1/32
    Pause64 = 0xa6, // Pause for 1/64

    PlaySpeed50  = 0xb0, // Play at ~50 bpm
    PlaySpeed60  = 0xb1, // Play at ~60 bpm
    PlaySpeed70  = 0xb2, // Play at ~70 bpm
    PlaySpeed80  = 0xb3, // Play at ~80 bpm
    PlaySpeed90  = 0xb4, // Play at ~90 bpm
    PlaySpeed100 = 0xb5, // Play at ~100 bpm
    PlaySpeed110 = 0xb6, // Play at ~110 bpm
    PlaySpeed120 = 0xb7, // Play at ~120 bpm (default)
    PlaySpeed130 = 0xb8, // Play at ~130 bpm
    PlaySpeed140 = 0xb9, // Play at ~140 bpm
    PlaySpeed150 = 0xba, // Play at ~150 bpm
    PlaySpeed160 = 0xbb, // Play at ~160 bpm
    PlaySpeed170 = 0xbc, // Play at ~170 bpm
    PlaySpeed180 = 0xbd, // Play at ~180 bpm
    PlaySpeed200 = 0xbe, // Play at ~200 bpm (for effects)
    PlaySpeed350 = 0xbf, // Play at ~350 bpm (for effects)
    
    NoteFadeOff    = 0xc0, // Stop any fading.
    NoteFadeUp1    = 0xc1, // Fade every note up at speed 1
    NoteFadeUp2    = 0xc2, // Fade every note up at speed 2
    NoteFadeUp3    = 0xc3, // Fade every note up at speed 3
    NoteFadeUp4    = 0xc4, // Fade every note up at speed 4
    NoteFadeUp5    = 0xc5, // Fade every note up at speed 5
    NoteFadeUp6    = 0xc6, // Fade every note up at speed 6
    NoteFadeUp7    = 0xc7, // Fade every note up at speed 7
    NoteFadeDown1  = 0xc9, // Fade every note down at speed 1
    NoteFadeDown2  = 0xca, // Fade every note down at speed 2
    NoteFadeDown3  = 0xcb, // Fade every note down at speed 3
    NoteFadeDown4  = 0xcc, // Fade every note down at speed 4
    NoteFadeDown5  = 0xcd, // Fade every note down at speed 5
    NoteFadeDown6  = 0xce, // Fade every note down at speed 6
    NoteFadeDown7  = 0xcf, // Fade every note down at speed 7
};


}


