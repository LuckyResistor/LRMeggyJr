//
// Sound Demo
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


const SoundToken PROGMEM melody1[] = {
    
    PlaySpeed90,
    
    // 3/8
    
    NoteE4, Play16,
    NoteDs4, Play16,
    // --:
    NoteE4, Play16,
    NoteDs4, Play16,
    NoteE4, Play16,
    NoteH3, Play16,
    NoteD4, Play16,
    NoteC4, Play16,
    // --
    NoteA3, Play8,
    Pause16,
    NoteC3, Play16,
    NoteE3, Play16,
    NoteA3, Play16,
    // --
    NoteH3, Play8,
    Pause16,
    NoteE3, Play16,
    NoteGs3, Play16,
    NoteH3, Play16,
    // --
    NoteC4, Play8,
    Pause16,
    NoteE3, Play16,
    NoteE4, Play16,
    NoteDs4, Play16,
    // --
    NoteE4, Play16,
    NoteDs4, Play16,
    NoteE4, Play16,
    NoteH3, Play16,
    NoteD4, Play16,
    NoteC4, Play16,
    // --
    NoteA3, Play8,
    Pause16,
    NoteC3, Play16,
    NoteE3, Play16,
    NoteA3, Play16,
    // --
    NoteH3, Play8,
    Pause16,
    NoteD3, Play16,
    NoteC4, Play16,
    NoteH3, Play16,
    // --
    NoteA3, Play4,
    
    SoundEnd // NEVER forget the end token!
};


const SoundToken PROGMEM melody2[] = {
    
    PlaySpeed120,

    NoteA3, Play16,
    Pause16,
    Pause8,
    Pause4,
    Pause2,
    NoteA3, Play16,
    Pause16,
    Pause8,
    Pause4,
    Pause4,
    Pause8,
    NoteC5, Play8,
    NoteA3, Play16,
    Pause16,
    Pause8,
    Pause4,
    Pause2,
    NoteA3, Play16,
    Pause16,
    Pause8,
    Pause4,
    Pause2,
    
    SoundEnd // NEVER forget the end token!
};


const SoundToken PROGMEM allNotes[] = {
    PlaySpeed120,
    
    NoteA0, Play16,
    NoteAs0, Play16,
    NoteH0, Play16,
    
    NoteC1, Play16,
    NoteCs1, Play16,
    NoteD1, Play16,
    NoteDs1, Play16,
    NoteE1, Play16,
    NoteF1, Play16,
    NoteFs1, Play16,
    NoteG1, Play16,
    NoteGs1, Play16,
    NoteA1, Play16,
    NoteAs1, Play16,
    NoteH1, Play16,
    
    NoteC2, Play16,
    NoteCs2, Play16,
    NoteD2, Play16,
    NoteDs2, Play16,
    NoteE2, Play16,
    NoteF2, Play16,
    NoteFs2, Play16,
    NoteG2, Play16,
    NoteGs2, Play16,
    NoteA2, Play16,
    NoteAs2, Play16,
    NoteH2, Play16,

    NoteC3, Play16,
    NoteCs3, Play16,
    NoteD3, Play16,
    NoteDs3, Play16,
    NoteE3, Play16,
    NoteF3, Play16,
    NoteFs3, Play16,
    NoteG3, Play16,
    NoteGs3, Play16,
    NoteA3, Play16,
    NoteAs3, Play16,
    NoteH3, Play16,
    
    NoteC4, Play16,
    NoteCs4, Play16,
    NoteD4, Play16,
    NoteDs4, Play16,
    NoteE4, Play16,
    NoteF4, Play16,
    NoteFs4, Play16,
    NoteG4, Play16,
    NoteGs4, Play16,
    NoteA4, Play16,
    NoteAs4, Play16,
    NoteH4, Play16,

    NoteC5, Play16,
    NoteCs5, Play16,
    NoteD5, Play16,
    NoteDs5, Play16,
    NoteE5, Play16,
    NoteF5, Play16,
    NoteFs5, Play16,
    NoteG5, Play16,
    NoteGs5, Play16,
    NoteA5, Play16,
    NoteAs5, Play16,
    NoteH5, Play16,
    
    NoteC6, Play16,
    NoteCs6, Play16,
    NoteD6, Play16,
    NoteDs6, Play16,
    NoteE6, Play16,
    NoteF6, Play16,
    NoteFs6, Play16,
    NoteG6, Play16,
    NoteGs6, Play16,
    NoteA6, Play16,
    NoteAs6, Play16,
    NoteH6, Play16,
    
    NoteC7, Play16,
    NoteCs7, Play16,
    NoteD7, Play16,
    NoteDs7, Play16,
    NoteE7, Play16,
    NoteF7, Play16,
    NoteFs7, Play16,
    NoteG7, Play16,
    NoteGs7, Play16,
    
    SoundEnd // NEVER forget the end token!
};



const SoundToken PROGMEM test440hz[] = {
    
    PlaySpeed80,
    
    NoteA3, Play1,
    NoteA4, Play1,
    NoteA5, Play1,
    NoteA6, Play1,
    
    NoteA0, Play1,
    NoteA1, Play1,
    NoteA2, Play1,
    
    SoundEnd // NEVER forget the end token!
};


const SoundToken PROGMEM effects[] = {
    
    PlaySpeed120,
    
    NoteA5, NoteFadeDown1, Play4,
    NoteA5, NoteFadeDown2, Play4,
    NoteA5, NoteFadeDown3, Play4,
    NoteA5, NoteFadeDown4, Play4,
    NoteA5, NoteFadeDown5, Play4,
    NoteA5, NoteFadeDown6, Play4,
    NoteA5, NoteFadeDown7, Play4,
    
    Pause4,
    
    NoteA3, NoteFadeUp1, Play4,
    NoteA3, NoteFadeUp2, Play4,
    NoteA3, NoteFadeUp3, Play4,
    NoteA3, NoteFadeUp4, Play4,
    NoteA3, NoteFadeUp5, Play4,
    NoteA3, NoteFadeUp6, Play4,
    NoteA3, NoteFadeUp7, Play4,

    Pause4,

    NoteA5, NoteFadeDown4, Play16,
    NoteE5, NoteFadeUp4, Play16,
    NoteA5, NoteFadeDown4, Play16,
    NoteE5, NoteFadeUp4, Play16,
    NoteA5, NoteFadeDown4, Play16,
    NoteE5, NoteFadeUp4, Play16,
    NoteA5, NoteFadeDown4, Play16,
    NoteE5, NoteFadeUp4, Play16,
    
    SoundEnd
};


// The setup code.
void setup() 
{
    meg.setup();
}


// The loop code.
void loop()
{
    const uint32_t frame = meg.frameSync();
    meg.fadePixel();
    
    // Play the sounds on button press.
    if (meg.isAButtonPressed()) {
        meg.playSound(melody1);
    }
    if (meg.isBButtonPressed()) {
        meg.playSound(melody2);
    }
    if (meg.isUpButtonPressed()) {
        meg.playSound(allNotes);
    }
    if (meg.isDownButtonPressed()) {
        meg.playSound(test440hz);
    }
    if (meg.isLeftButtonPressed()) {
        meg.playSound(effects);
    }
    
    // Visualize the played note.
    const uint8_t note = meg.getPlayedNote();
    if (note != 0) {
        meg.setPixel(note & 0x7, (note >> 3) & 0x7, Color::white());
    }
}
