//
// Lucky Resistor's MeggyJr Driver
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
#include "LRMeggyJr.h"


// Details about the mapping from the Hardware to the Controller
// ---------------------------------------------------------------------------
//
// Port D - 0 => (RxD In  Serial)
//        - 1 => (TxD Out Serial)
//        - 2 => Row 1
//        - 3 => Row 2
//        - 4 => Row 3
//        - 5 => Row 4
//        - 6 => Row 5
//        - 7 => Row 6
// Port B - 0 => Row 7
//      B - 4 => Row 8
//
// If you enable one of the outputs for the row, by setting the output to 0,
// the LEDs of the selected colums are enabled. The columns are controlled by
// the LED driver chips. They just are two 16 bit shift registers which
// are chained.
//
// Driver Out 0: 0 => Column 0
// Driver Out 0: 1 => Column 1
// ...                ...
// Driver Out 0:15 => Column 15
// Driver Out 1: 0 => Column 16
// Driver Out 1: 1 => Column 17
// ...                ...
// Driver Out 1: 7 => Column 23
// Driver Out 1: 8 => Extra LED 0
// Driver Out 1: 9 => Extra LED 1
// ...                ...
// Driver Out 1:14 => Extra LED 6
// Driver Out 1:15 => Extra LED 7
//
//         Rows
//    8 7 6 5 4 3 2 1
//    O O O O O O O O   7
//    O O O O O O O O   6 Columns
//    O O O O O O O O   5
//    O O O O O O O O   4   Frame bits:
//    O O O O O O O O   3   byte 0: Extra
//    O O O O O O O O   2   byte 1: Red
//    O O O O O O O O   1   byte 2: Green
//    O O O O O O O O   0   Byte 3: Blue
//
// The Speaker is connected to the output of Port B - 1, which can be
// controlled Using OCA1 to generated tones.
//

namespace lr {
    

// An anonymous namespace for the used variables and internal functions.
namespace {

    
// Constants
// ---------------------------------------------------------------------------
    
// The number of rows
const uint8_t numberOfRows = 8;
    
// The mask for the rows
const uint8_t numberOfRowMask = 0x07;
    
// The number of brightness levels
const uint8_t brightnessLevels = 16;

// The mask for the brightness levels.
const uint8_t brightnessLevelsMask = 0x0F;
    
// The size of the matrix in bytes
const uint8_t ledMatrixSize = 96;
    
// The size of a row in the matrix
const uint8_t ledMatrixRowSize = 12;
    
// The LED matrix
// ---------------------------------------------------------------------------
    
// A matrix with the colors for all 64 LEDs.
// Each row has the format RG BR GB  RG BR GB  RG BR GB  RG BR GB.
uint8_t ledMatrix[ledMatrixSize];
    
// The led matrix which is actually dislayed (backbuffer)
uint8_t displayedLedMatrix[ledMatrixSize];

// A matrix with for the 8 external LEDs
uint8_t extLedMatrix;
    
// Required variables for the LED driver
// ---------------------------------------------------------------------------

// The current driven row
uint8_t drivenRow;

// The current brightnes value
uint8_t drivenBrightness;
    
// The next bits for the row.
uint8_t drivenBits[3];
    
// The driven frame, which is used as divisor for the application frame rate.
uint8_t drivenFrame;
    
// The row masks for port B and D in program memory.
const uint8_t drivenRowPortD[numberOfRows] PROGMEM = {
    B11111111,
    B11111111,
    B01111111,
    B10111111,
    B11011111,
    B11101111,
    B11110111,
    B11111011,
};
const uint8_t drivenRowPortB[numberOfRows] PROGMEM = {
    B11101111,
    B11111110,
    B11111111,
    B11111111,
    B11111111,
    B11111111,
    B11111111,
    B11111111,    
};

// The application frame values
// ---------------------------------------------------------------------------

// The frame rate divisor for the application.
uint8_t applicationFrameRate;
    
// The current application frame
uint32_t applicationFrame;

// The flag for frame synchronization.
volatile uint8_t applicationFrameSync;
    
// The last measured time in microseconds to measure the load
uint32_t applicationFrameSyncLastTime;

// The measured time for an application frame
uint32_t applicationFrameDuration;

// The state of the measurement process.
enum ApplicationFrameMeasureState : uint8_t {
    ApplicationFrameMeasure_Uninitialized,
    ApplicationFrameMeasure_LastTime,
    ApplicationFrameMeasure_Ready
} applicationFrameMeasureState;

    
// Variables for the sound player.
// ---------------------------------------------------------------------------

// The pointer to the next sound token.
const uint8_t *nextSoundToken;
    
// The current speed of a 1/64 note of the sound.
uint8_t soundSpeed;

// The timer for the sound. Counts up to speed.
uint8_t soundTimer;

// The current state
enum SoundState : uint8_t {
    SoundIdle, // There is no sound to play.
    SoundPlaying, // The sound is playing at the choosen frequency.
    SoundPause, // A pause is "played".
    SoundStopRequest, // A stop of all sound is requested.
    SoundNewRequest, // A new sound was started.
    SoundDisabled, // All sound is disabled.
} soundState;

// The current sound duration 64 = 1/1
uint8_t currentSoundDuration;

// The current note of the sound.
uint8_t currentNote;
    
// The state for fading.
// 0 = no fading.
// bit 6+7: 00 = fade up, 10 = fade down.
// bit 0-3: fade speed
uint8_t soundFadeState;
    
    
// Variables for button handling.
// ---------------------------------------------------------------------------
    
// The current state of the buttons for the application frame.
uint8_t currentButtonState;

// The last state of the buttons from last application frame.
uint8_t lastButtonState;

    

// The Sound Driver
// ----------------------------------------------------------------------------
// Sound driver is called at 1.92kHz = every 0.00052s or 0.52ms
//
// Shortest note is 1/64 which always defines the speed value.
// 120 bpm => 1/1 = 0.5s => 1/64 = 0.008s => ~15 calls. for 4/4 = 60
//
// Frequency:
// Formula is: pow(2.0, ((tn - 49.0) / 12.0)) * 440.0
// Where tn = key => 1 = A-0, 2 = A#0, 3 = H-0, 4 = C-1 ...

// These are the base frequencies used to calculate the final one.
const uint16_t soundFreqBase[] PROGMEM = {
    36364, // a-3     220.0Hz
    34323, // a#3     233.1Hz
    32396, // h-3     246.9Hz
    30578, // c-3     261.6Hz
    28862, // c#3     277.2Hz
    27242, // d-3     293.7Hz
    25713, // d#3     311.1Hz
    24270, // e-3     329.6Hz
    22908, // f-3     349.2Hz
    21622, // f#3     370.0Hz
    20408, // g-3     392.0Hz
    19263, // g#3     415.3Hz
};

// The modification for each octave
const uint8_t soundFreqMod[] PROGMEM = {
    0x12, // octave 0 : Prescaler 1/8 - >>1
    0x22, // octave 1 : Prescaler 1/8 - >>2
    0x01, // octave 2 : Prescaler 1/1 - >>0
    0x11, // octave 3 : Prescaler 1/1 - >>1
    0x21, // octave 4 : Prescaler 1/1 - >>2
    0x31, // octave 5 : Prescaler 1/1 - >>3
    0x41, // octave 6 : Prescaler 1/1 - >>4
    0x51, // octave 7 : Prescaler 1/1 - >>5
};
    
// The sound speeds
const uint8_t soundSpeedValues[] PROGMEM = {
    142, // bpm=50.2    ~50
    119, // bpm=59.9    ~60
    102, // bpm=69.9    ~70
    89,  // bpm=80.1    ~80
    79,  // bpm=90.2    ~90
    71,  // bpm=100.4  ~100
    65,  // bpm=109.6  ~110
    59,  // bpm=120.8  ~120
    55,  // bpm=129.5  ~130
    51,  // bpm=139.7  ~140
    48,  // bpm=148.4  ~150
    45,  // bpm=158.3  ~160
    42,  // bpm=169.6  ~170
    40,  // bpm=178.1  ~180
    35,  // bpm=203.6  ~200
    20,  // bpm=356.2  ~350
};

    
// Sound off
static void soundOff()
{
    // Set the timer to off
    TCCR1A = 0;
    TCCR1B = 0;
    DDRB &= ~(_BV(DDB1)); // Set the speaker i/o to input
    PORTB |= _BV(PORTB1); // Set the output to low.
}


// Sound on
static void soundOn()
{
    // Initialize the timer
    // - Toggle OC1A/B on compare match.
    // - PWM, Phase and Frequency Correct
    TCCR1A = _BV(COM1A0)|_BV(WGM10);
    DDRB |= _BV(DDB1); // Set the speaker i/o to output
}


// Read the next token from the sound.
static uint8_t soundReadNextToken()
{
    if (nextSoundToken == 0) {
        return SoundEnd;
    } else {
        const uint8_t token = pgm_read_byte(nextSoundToken);
        ++nextSoundToken;
        return token;
    }
}
    
    
// Parse the next token.
// Returns true if a next token can be read.
static bool soundParseNextToken()
{
    const uint8_t token = soundReadNextToken();
    if (token == SoundEnd) {
        soundOff();
        soundState = SoundIdle;
        nextSoundToken = 0;
        return false;
    } else if (token >= NoteA0 && token <= NoteGs7) {
        // Store the note
        currentNote = token - NoteA0; // note index starting from 0
        return true;
    } else if (token >= Play1 && token <= Play64) {
        // Play a note at the given length
        soundTimer = soundSpeed;
        currentSoundDuration = (64 >> (token - Play1));
        // Calculate the timer settings to set the frequency
        uint16_t timerTop = pgm_read_word(&soundFreqBase[currentNote%12]);
        uint8_t modifier = pgm_read_byte(&soundFreqMod[currentNote/12]);
        timerTop >>= (modifier >> 4); // use upper nibble of modifier for shift.
        TCCR1B = _BV(WGM13) | (modifier & B00000111); // set prescaling.
        OCR1A = timerTop; // Set timer top.
        if (soundState != SoundPlaying) {
            soundOn();
        }
        soundState = SoundPlaying;
        return false;
    } else if (token >= Pause1 && token <= Pause64) {
        soundTimer = soundSpeed;
        currentSoundDuration = (64 >> (token - Pause1));
        if (soundState == SoundPlaying) {
            soundOff();
        }
        soundState = SoundPause;
        return false;
    } else if (token >= PlaySpeed50 && token <= PlaySpeed180) {
        // Set the sound speed to the right value.
        soundSpeed = pgm_read_byte(&soundSpeedValues[token-PlaySpeed50]);
        soundTimer = soundSpeed; // reset the sound timer, just in case.
        return true;
    } else if (token == NoteFadeOff) {
        soundFadeState = 0;
    } else if (token >= NoteFadeUp1 && token <= NoteFadeUp7) {
        soundFadeState = token - NoteFadeUp1 + 1;
    } else if (token >= NoteFadeDown1 && token <= NoteFadeDown7) {
        soundFadeState = (token - NoteFadeDown1 + 1) | 0x80;
    } else {
        // Skip any unknown token
        return true;
    }
}


// The setup for the sound driver
static void soundDriverSetup()
{
    // Initialize the variables
    nextSoundToken = 0;
    soundState = SoundIdle;
    currentSoundDuration = 0;
    currentNote = 20;
    soundSpeed = 59; // ~120 bpm
    soundTimer = soundSpeed;
    soundFadeState = 0;
    soundOff();
}


// The sound driver, called at 1.9kHz.
static void soundDriver()
{
    switch (soundState) {
        case SoundPlaying:
            // same as pause + fading
            if (soundFadeState != 0) {
                const uint16_t value = OCR1A;
                if ((soundFadeState & B11000000) == B10000000) {
                    // down
                    if (value < 0xfff0) {
                        OCR1A = value + (soundFadeState & B00001111);
                    }
                } else {
                    // up
                    if (value > 0x000f) {
                        OCR1A = value - (soundFadeState & B00001111);
                    }
                }
            }
        case SoundPause:
            // Wait for a 1/64 note.
            if (--soundTimer == 0) { // testing for 0 is always faster.
                PORTC &= ~(_BV(5));
                soundTimer = soundSpeed;
                PORTC |= _BV(5);
                // Check if the current note is still played.
                if (--currentSoundDuration == 0) {
                    // If yes, parse the next tokens.
                    while (soundParseNextToken()) {}
                    break;
                }
            }
            break;
            
        case SoundStopRequest:
            nextSoundToken = 0;
            soundFadeState = 0;
            soundState = SoundIdle;
            soundOff();
            break;
            
        case SoundNewRequest:
            soundSpeed = 59; // ~120 bpm
            soundFadeState = 0;
            soundState = SoundIdle;
            while (soundParseNextToken()) {}
            break;

        case SoundIdle:
        case SoundDisabled:
            break;
    }
}

    
// The LED Driver
// ----------------------------------------------------------------------------

    
// Turn the complete display off.
static void displayOff()
{
    PORTD |= B11111100;
    PORTB |= B00010001;
}


// The setup method for the LED driver.
static void ledDriverSetup()
{
    // Start with row 0 and brightnesss 0;
    drivenRow = 0;
    drivenBrightness = 0;
    drivenBits[0] = 0;
    drivenBits[1] = 0;
    drivenBits[2] = 0;
    
    // Start with a black frame.
    for (uint8_t i = 0; i < ledMatrixSize; ++i) {
        displayedLedMatrix[i] = 0x00;
    }
    
    // set the driven frame to 0
    drivenFrame = 0;
}

// This copies the "ledMatrix" into "displayLedMatrix" and then
// precalculates the bits for the next row.
static void ledDriverCopyDisplay()
{
    // This is always row 7 and brightness 15
    // (calculated brightness 0 for next row)
    
    // prepare the pointer
    uint32_t *source = reinterpret_cast<uint32_t*>(ledMatrix);
    uint32_t *destination = reinterpret_cast<uint32_t*>(displayedLedMatrix);
    
    // Enable SPI (SPE) in master mode (MSTR).
    SPCR = _BV(SPE)|_BV(MSTR);
 
    // Send first byte. First byte is the external LED.
    SPDR = B00000000;
    
    // Copy some bytes (2 rows)
    *destination++ = *source++;
    *destination++ = *source++;
    *destination++ = *source++;
    
    *destination++ = *source++;
    *destination++ = *source++;
    *destination++ = *source++;

    // Send byte 2:
    SPDR = drivenBits[0];

    // Copy some bytes (2 rows)
    *destination++ = *source++;
    *destination++ = *source++;
    *destination++ = *source++;
    
    *destination++ = *source++;
    *destination++ = *source++;
    *destination++ = *source++;

    // Send byte 3:
    SPDR = drivenBits[1];
    
    // Copy some bytes (2 rows)
    *destination++ = *source++;
    *destination++ = *source++;
    *destination++ = *source++;
    
    *destination++ = *source++;
    *destination++ = *source++;
    *destination++ = *source++;

    // Send byte 4:
    SPDR = drivenBits[2];
    
    // Copy some bytes (2 rows)
    *destination++ = *source++;
    *destination++ = *source++;
    *destination++ = *source++;
    
    *destination++ = *source++;
    *destination++ = *source++;
    *destination++ = *source++;
    
    // Latch pulse
    PORTB |= _BV(2);
    PORTB &= ~_BV(2);
    
    // Turn SPI off
    SPCR = B00000000;
    
    // Now we copied the display, but have not yet precalculated the next row.
    uint8_t r = 0; // Red bits
    uint8_t g = 0; // Green bits
    uint8_t b = 0; // Blue bits

    // Turn the correct row (delayed to avoid shadows)
    PORTD &= pgm_read_byte(&drivenRowPortD[drivenRow]);
    PORTB &= pgm_read_byte(&drivenRowPortB[drivenRow]);
    
    // A pointer to the next row in the matrix.
    uint8_t *lm = displayedLedMatrix;
    
    // The brightness value
    uint8_t cb = 0;
    
    asm volatile(
    "mov r16, %[lm0]"   "\n\t"
    "swap r16"          "\n\t"
    "andi r16, 0x0F"    "\n\t"
    "cp %[cb], r16"     "\n\t"
    "rol %[r]"          "\n\t"
    "andi %[lm0], 0x0F" "\n\t"
    "cp %[cb], %[lm0]"  "\n\t"
    "rol %[g]"          "\n\t"

    "mov r16, %[lm1]"   "\n\t"
    "swap r16"          "\n\t"
    "andi r16, 0x0F"    "\n\t"
    "cp %[cb], r16"     "\n\t"
    "rol %[b]"          "\n\t"
    "andi %[lm1], 0x0F" "\n\t"
    "cp %[cb], %[lm1]"  "\n\t"
    "rol %[r]"          "\n\t"

    "mov r16, %[lm2]"   "\n\t"
    "swap r16"          "\n\t"
    "andi r16, 0x0F"    "\n\t"
    "cp %[cb], r16"     "\n\t"
    "rol %[g]"          "\n\t"
    "andi %[lm2], 0x0F" "\n\t"
    "cp %[cb], %[lm2]"  "\n\t"
    "rol %[b]"          "\n\t"

    : [r] "+r" (r), [g] "+r" (g), [b] "+r" (b)
    : [lm0] "d" (lm[0]), [lm1] "d" (lm[1]), [lm2] "d" (lm[2]), [cb] "r" (cb)
    : "r16"
    );
    asm volatile(
    "mov r16, %[lm0]"   "\n\t"
    "swap r16"          "\n\t"
    "andi r16, 0x0F"    "\n\t"
    "cp %[cb], r16"     "\n\t"
    "rol %[r]"          "\n\t"
    "andi %[lm0], 0x0F" "\n\t"
    "cp %[cb], %[lm0]"  "\n\t"
    "rol %[g]"          "\n\t"

    "mov r16, %[lm1]"   "\n\t"
    "swap r16"          "\n\t"
    "andi r16, 0x0F"    "\n\t"
    "cp %[cb], r16"     "\n\t"
    "rol %[b]"          "\n\t"
    "andi %[lm1], 0x0F" "\n\t"
    "cp %[cb], %[lm1]"  "\n\t"
    "rol %[r]"          "\n\t"

    "mov r16, %[lm2]"   "\n\t"
    "swap r16"          "\n\t"
    "andi r16, 0x0F"    "\n\t"
    "cp %[cb], r16"     "\n\t"
    "rol %[g]"          "\n\t"
    "andi %[lm2], 0x0F" "\n\t"
    "cp %[cb], %[lm2]"  "\n\t"
    "rol %[b]"          "\n\t"

    : [r] "+r" (r), [g] "+r" (g), [b] "+r" (b)
    : [lm0] "d" (lm[3]), [lm1] "d" (lm[4]), [lm2] "d" (lm[5]), [cb] "r" (cb)
    : "r16"
    );
    asm volatile(
    "mov r16, %[lm0]"   "\n\t"
    "swap r16"          "\n\t"
    "andi r16, 0x0F"    "\n\t"
    "cp %[cb], r16"     "\n\t"
    "rol %[r]"          "\n\t"
    "andi %[lm0], 0x0F" "\n\t"
    "cp %[cb], %[lm0]"  "\n\t"
    "rol %[g]"          "\n\t"

    "mov r16, %[lm1]"   "\n\t"
    "swap r16"          "\n\t"
    "andi r16, 0x0F"    "\n\t"
    "cp %[cb], r16"     "\n\t"
    "rol %[b]"          "\n\t"
    "andi %[lm1], 0x0F" "\n\t"
    "cp %[cb], %[lm1]"  "\n\t"
    "rol %[r]"          "\n\t"

    "mov r16, %[lm2]"   "\n\t"
    "swap r16"          "\n\t"
    "andi r16, 0x0F"    "\n\t"
    "cp %[cb], r16"     "\n\t"
    "rol %[g]"          "\n\t"
    "andi %[lm2], 0x0F" "\n\t"
    "cp %[cb], %[lm2]"  "\n\t"
    "rol %[b]"          "\n\t"

    : [r] "+r" (r), [g] "+r" (g), [b] "+r" (b)
    : [lm0] "d" (lm[6]), [lm1] "d" (lm[7]), [lm2] "d" (lm[8]), [cb] "r" (cb)
    : "r16"
    );
    asm volatile(
    "mov r16, %[lm0]"   "\n\t"
    "swap r16"          "\n\t"
    "andi r16, 0x0F"    "\n\t"
    "cp %[cb], r16"     "\n\t"
    "rol %[r]"          "\n\t"
    "andi %[lm0], 0x0F" "\n\t"
    "cp %[cb], %[lm0]"  "\n\t"
    "rol %[g]"          "\n\t"

    "mov r16, %[lm1]"   "\n\t"
    "swap r16"          "\n\t"
    "andi r16, 0x0F"    "\n\t"
    "cp %[cb], r16"     "\n\t"
    "rol %[b]"          "\n\t"
    "andi %[lm1], 0x0F" "\n\t"
    "cp %[cb], %[lm1]"  "\n\t"
    "rol %[r]"          "\n\t"

    "mov r16, %[lm2]"   "\n\t"
    "swap r16"          "\n\t"
    "andi r16, 0x0F"    "\n\t"
    "cp %[cb], r16"     "\n\t"
    "rol %[g]"          "\n\t"
    "andi %[lm2], 0x0F" "\n\t"
    "cp %[cb], %[lm2]"  "\n\t"
    "rol %[b]"          "\n\t"

    : [r] "+r" (r), [g] "+r" (g), [b] "+r" (b)
    : [lm0] "d" (lm[9]), [lm1] "d" (lm[10]), [lm2] "d" (lm[11]), [cb] "r" (cb)
    : "r16"
    );
    
    // Store the calculated bits for the next row:
    drivenBits[0] = r;
    drivenBits[1] = g;
    drivenBits[2] = b;
}

    
// This displays a normal row. And precalculates the bits for the next row.
static void ledDriverNormalRow()
{
    // While the data is sent via SPI, the bits for the next row are calculated.
    uint8_t r = 0; // Red bits
    uint8_t g = 0; // Green bits
    uint8_t b = 0; // Blue bits
    
    // A pointer to the next row in the matrix.
    uint8_t *lm = &displayedLedMatrix[((drivenRow+1)&numberOfRowMask)*ledMatrixRowSize];
    
    // The brightness value
    uint8_t cb = drivenBrightness;
    if (drivenRow == (numberOfRows-1)) {
        // Because we calculate always the bits for the next row, we have to
        // adjust the brightness level for the last row, because this is already
        // the next one with different brigtness.
        ++cb;
        cb &= brightnessLevelsMask;
    }
    
    // Enable SPI (SPE) in master mode (MSTR).
    SPCR = _BV(SPE)|_BV(MSTR);
    
    // Send first byte. First byte is the external LED.
    if (drivenRow == 0) {
        SPDR = extLedMatrix;
    } else {
        SPDR = B00000000;
    }
    
    // White the first byte is sent, calculate some bits
    asm volatile(
    "mov r16, %[lm0]"   "\n\t"
    "swap r16"          "\n\t"
    "andi r16, 0x0F"    "\n\t"
    "cp %[cb], r16"     "\n\t"
    "rol %[r]"          "\n\t"
    "andi %[lm0], 0x0F" "\n\t"
    "cp %[cb], %[lm0]"  "\n\t"
    "rol %[g]"          "\n\t"

    "mov r16, %[lm1]"   "\n\t"
    "swap r16"          "\n\t"
    "andi r16, 0x0F"    "\n\t"
    "cp %[cb], r16"     "\n\t"
    "rol %[b]"          "\n\t"
    "andi %[lm1], 0x0F" "\n\t"
    "cp %[cb], %[lm1]"  "\n\t"
    "rol %[r]"          "\n\t"

    "mov r16, %[lm2]"   "\n\t"
    "swap r16"          "\n\t"
    "andi r16, 0x0F"    "\n\t"
    "cp %[cb], r16"     "\n\t"
    "rol %[g]"          "\n\t"
    "andi %[lm2], 0x0F" "\n\t"
    "cp %[cb], %[lm2]"  "\n\t"
    "rol %[b]"          "\n\t"

    : [r] "+r" (r), [g] "+r" (g), [b] "+r" (b)
    : [lm0] "d" (lm[0]), [lm1] "d" (lm[1]), [lm2] "d" (lm[2]), [cb] "r" (cb)
    : "r16"
    );
    
    // Send byte 2:
    SPDR = drivenBits[0];
    
    // While byte 2 is sent, calculate some bits.
    asm volatile(
    "mov r16, %[lm0]"   "\n\t"
    "swap r16"          "\n\t"
    "andi r16, 0x0F"    "\n\t"
    "cp %[cb], r16"     "\n\t"
    "rol %[r]"          "\n\t"
    "andi %[lm0], 0x0F" "\n\t"
    "cp %[cb], %[lm0]"  "\n\t"
    "rol %[g]"          "\n\t"

    "mov r16, %[lm1]"   "\n\t"
    "swap r16"          "\n\t"
    "andi r16, 0x0F"    "\n\t"
    "cp %[cb], r16"     "\n\t"
    "rol %[b]"          "\n\t"
    "andi %[lm1], 0x0F" "\n\t"
    "cp %[cb], %[lm1]"  "\n\t"
    "rol %[r]"          "\n\t"

    "mov r16, %[lm2]"   "\n\t"
    "swap r16"          "\n\t"
    "andi r16, 0x0F"    "\n\t"
    "cp %[cb], r16"     "\n\t"
    "rol %[g]"          "\n\t"
    "andi %[lm2], 0x0F" "\n\t"
    "cp %[cb], %[lm2]"  "\n\t"
    "rol %[b]"          "\n\t"

    : [r] "+r" (r), [g] "+r" (g), [b] "+r" (b)
    : [lm0] "d" (lm[3]), [lm1] "d" (lm[4]), [lm2] "d" (lm[5]), [cb] "r" (cb)
    : "r16"
    );
    
    // Send byte 3:
    SPDR = drivenBits[1];
    
    // While byte 3 is sent, calculate some bits.
    asm volatile(
    "mov r16, %[lm0]"   "\n\t"
    "swap r16"          "\n\t"
    "andi r16, 0x0F"    "\n\t"
    "cp %[cb], r16"     "\n\t"
    "rol %[r]"          "\n\t"
    "andi %[lm0], 0x0F" "\n\t"
    "cp %[cb], %[lm0]"  "\n\t"
    "rol %[g]"          "\n\t"

    "mov r16, %[lm1]"   "\n\t"
    "swap r16"          "\n\t"
    "andi r16, 0x0F"    "\n\t"
    "cp %[cb], r16"     "\n\t"
    "rol %[b]"          "\n\t"
    "andi %[lm1], 0x0F" "\n\t"
    "cp %[cb], %[lm1]"  "\n\t"
    "rol %[r]"          "\n\t"

    "mov r16, %[lm2]"   "\n\t"
    "swap r16"          "\n\t"
    "andi r16, 0x0F"    "\n\t"
    "cp %[cb], r16"     "\n\t"
    "rol %[g]"          "\n\t"
    "andi %[lm2], 0x0F" "\n\t"
    "cp %[cb], %[lm2]"  "\n\t"
    "rol %[b]"          "\n\t"

    : [r] "+r" (r), [g] "+r" (g), [b] "+r" (b)
    : [lm0] "d" (lm[6]), [lm1] "d" (lm[7]), [lm2] "d" (lm[8]), [cb] "r" (cb)
    : "r16"
    );
    
    // Send byte 4:
    SPDR = drivenBits[2];
    
    // While byte 4 is sent, calculate some bits.
    asm volatile(
    "mov r16, %[lm0]"   "\n\t"
    "swap r16"          "\n\t"
    "andi r16, 0x0F"    "\n\t"
    "cp %[cb], r16"     "\n\t"
    "rol %[r]"          "\n\t"
    "andi %[lm0], 0x0F" "\n\t"
    "cp %[cb], %[lm0]"  "\n\t"
    "rol %[g]"          "\n\t"

    "mov r16, %[lm1]"   "\n\t"
    "swap r16"          "\n\t"
    "andi r16, 0x0F"    "\n\t"
    "cp %[cb], r16"     "\n\t"
    "rol %[b]"          "\n\t"
    "andi %[lm1], 0x0F" "\n\t"
    "cp %[cb], %[lm1]"  "\n\t"
    "rol %[r]"          "\n\t"

    "mov r16, %[lm2]"   "\n\t"
    "swap r16"          "\n\t"
    "andi r16, 0x0F"    "\n\t"
    "cp %[cb], r16"     "\n\t"
    "rol %[g]"          "\n\t"
    "andi %[lm2], 0x0F" "\n\t"
    "cp %[cb], %[lm2]"  "\n\t"
    "rol %[b]"          "\n\t"

    : [r] "+r" (r), [g] "+r" (g), [b] "+r" (b)
    : [lm0] "d" (lm[9]), [lm1] "d" (lm[10]), [lm2] "d" (lm[11]), [cb] "r" (cb)
    : "r16"
    );
    
    // Latch pulse
    PORTB |= _BV(2);
    PORTB &= ~_BV(2);
    
    // Turn SPI off
    SPCR = B00000000;
    
    // Store the calculated bits for the next row:
    drivenBits[0] = r;
    drivenBits[1] = g;
    drivenBits[2] = b;

    // Turn the correct row on
    PORTD &= pgm_read_byte(&drivenRowPortD[drivenRow]);
    PORTB &= pgm_read_byte(&drivenRowPortB[drivenRow]);
}
    
    
// The LED driver.
static void ledDriver()
{
    // Turn the display off, because column bits get shifted.
    displayOff();
    
    // For the last row, after a complete brigthness loop, a
    // special handling is performed. Depending on the application
    // frame rate, the "ledMatrix" is copied into the "displayMatrix".
    
    if (drivenRow == (numberOfRows-1) && drivenBrightness == (brightnessLevels-1)) {
        // Manage the application frame.
        if (++drivenFrame >= applicationFrameRate) {
            // The special case where we copy the "ledMatrix".
            ledDriverCopyDisplay();
            drivenFrame = 0;
            ++applicationFrame;
            // signal the frame sync.
            ++applicationFrameSync;
            // see if we have to measure a frame
            if (applicationFrameMeasureState == ApplicationFrameMeasure_Uninitialized) {
                applicationFrameSyncLastTime = micros();
                applicationFrameMeasureState = ApplicationFrameMeasure_LastTime;
            } else if (applicationFrameMeasureState == ApplicationFrameMeasure_LastTime) {
                applicationFrameDuration = micros() - applicationFrameSyncLastTime;
                applicationFrameSyncLastTime = micros();
                applicationFrameMeasureState = ApplicationFrameMeasure_Ready;
            }
            // Manage the button states
            lastButtonState = currentButtonState;
            currentButtonState = (~(PINC) & B00111111);
        } else {
            ledDriverNormalRow();
        }
    } else {
        // the regular case.
        ledDriverNormalRow();
    }

    // Call the sound driver at 1.92kHz.
    // But never at the same time as the display copy.
    if (drivenRow == (numberOfRows-2)) {
        soundDriver();
    }
    
    // After the last row, increase the brightness level.
    if (drivenRow == (numberOfRows-1)) {
        ++drivenBrightness;
        drivenBrightness &= brightnessLevelsMask;
    }
    
    // Increase the driven row
    ++drivenRow;
    drivenRow &= numberOfRowMask; // limit to 8 rows.
}
    

}
    

// The implementation of the MeggyJr class
// ----------------------------------------------------------------------------

    
// The global instance of the interface.
MeggyJr meg;


void MeggyJr::setup(FrameRate frameRate)
{
    // 1. Setup the ports.
    //    Port C, all inputs with pull-ups.
    DDRC  = B00100000; // all inputs.
    PORTC = B11111111; // pull ups.
    //    Port D, all output except Rx, initialize Low
    DDRD  = B11111110;
    PORTD = B11111110;
    //    Port B, rest outputs, initialize low.
    DDRB  = B00111111;
    PORTB = B11111111;
    
    // 2. Turn the display off
    displayOff();
    
    // 3. Set an initial state for the display.
    clear();

    // 4. Initialize the driver.
    ledDriverSetup();
    
    // 5. Initialize the SPI bus.
    SPSR = _BV(SPI2X); // Enable double SPI speed.
    SPCR = B00000000; // Disable the SPI bus.

    // 6. Initialize the interrupt for the LEDs using timer 2.
    TCCR2A = _BV(WGM21); // OC0A/B and OC2A/B disconnected, CTC mode, TOP = OCRA
    TCCR2B = _BV(CS21); // Use main clock 1/8 prescale.
    //    Set the speed of the timer (set TOP).
    OCR2A = F_CPU / 8 /*prescale*/ / 8 /*rows*/ / 16 /*levels*/ / 120 /*FPS*/;
    TIMSK2 = _BV(OCIE2A); // Enable interrupt from timer 2 compare.
    
    // 7. Set the application frame to 0 and set all sync values to 0.
    applicationFrame = 0;
    applicationFrameDuration = 0;
    applicationFrameSyncLastTime = 0;
    applicationFrameMeasureState = ApplicationFrameMeasure_Uninitialized;
    
    // 8. Set the frame rate for the application.
    applicationFrameRate = frameRate;
    
    // 9. Initialize button states.
    currentButtonState = 0;
    lastButtonState = 0;
    
    // 10. Initialize sound
    soundDriverSetup();
    
    // 11. Enable interrupts.
    sei();
}

    
// The interrupt function to drive the LEDs.
SIGNAL(TIMER2_COMPA_vect)
{
    ledDriver();
}

 
void MeggyJr::clear()
{
    memset(ledMatrix, 0, ledMatrixSize);
    extLedMatrix = 0;
}


void MeggyJr::setExtraLeds(uint8_t bits)
{
    extLedMatrix = bits;
}

    
uint8_t MeggyJr::getExtraLeds() const
{
    return extLedMatrix;
}
    
    
void MeggyJr::enableExtraLed(uint8_t index)
{
    extLedMatrix |= _BV(index);
}

    
void MeggyJr::disableExtraLed(uint8_t index)
{
    extLedMatrix &= ~(_BV(index));
}

    
bool MeggyJr::isExtraLedEnabled(uint8_t index) const
{
    return (extLedMatrix & _BV(index)) != 0;
}
    

void MeggyJr::clearPixels()
{
    memset(ledMatrix, 0, ledMatrixSize);
}
    
    
void MeggyJr::setPixel(uint8_t x, uint8_t y, const Color &color)
{
    const uint16_t c = color._color;
    uint8_t* const target = &ledMatrix[(y>>1)*3+x*ledMatrixRowSize];
    if ((y & 1) == 0) {
        target[0] = c >> 4;
        target[1] = (target[1] & 0x0F) | ((c << 4) & 0xFF);
    } else {
        target[1] = (target[1] & 0xF0) | (c >> 8);
        target[2] = c & 0xFF;
    }
}

    
void MeggyJr::setPixelS(uint8_t x, uint8_t y, const Color &color)
{
    if (x<8 && y<8) {
        setPixelS(x, y, color);
    }
}
    
    
Color MeggyJr::getPixel(uint8_t x, uint8_t y) const
{
    const uint8_t* const target = &ledMatrix[(y>>1)*3+x*ledMatrixRowSize];
    if ((y & 1) == 0) {
        return Color(target[0] >> 4, target[0] & 0x0F, target[1] >> 4);
    } else {
        return Color(target[1] & 0x0F, target[2] >> 4, target[2] & 0x0F);
    }
}
    

Color MeggyJr::getPixelS(uint8_t x, uint8_t y) const
{
    if (x<8 && y<8) {
        return getPixel(x, y);
    } else {
        return Color::black();
    }
}

    
void MeggyJr::fillRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const Color &color)
{
    for (uint8_t xd = 0; xd < width; ++xd) {
        for (uint8_t yd = 0; yd < height; ++yd) {
            setPixel(x+xd, y+yd, color);
        }
    }
}

    
void MeggyJr::fillRectS(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const Color &color)
{
    for (uint8_t xd = 0; xd < width; ++xd) {
        for (uint8_t yd = 0; yd < height; ++yd) {
            setPixelS(x+xd, y+yd, color);
        }
    }
}

    
void MeggyJr::scrollPixel(ScrollDirection scrollDirection)
{
    // While scrolling left and right is just moving bytes around,
    // scrolling up and down requires a bit shift which is way to
    // slow in C++. Nice to have a RISC with lots of registers.
    switch (scrollDirection) {
        case ScrollLeft:
        {
            uint8_t pixels[ledMatrixRowSize];
            memcpy(pixels, ledMatrix, ledMatrixRowSize);
            memmove(ledMatrix, ledMatrix+ledMatrixRowSize, ledMatrixRowSize*7);
            memcpy(ledMatrix+(7*ledMatrixRowSize), pixels, ledMatrixRowSize);
        }
        break;
        
        case ScrollRight:
        {
            uint8_t pixels[ledMatrixRowSize];
            memcpy(pixels, ledMatrix+(7*ledMatrixRowSize), ledMatrixRowSize);
            memmove(ledMatrix+ledMatrixRowSize, ledMatrix, ledMatrixRowSize*7);
            memcpy(ledMatrix, pixels, ledMatrixRowSize);
        }
        break;
            
        case ScrollUp:
        {
            // Asm necessary because of speed.
            // rol seems slow, but swap needs and and mov.
            uint8_t *p = ledMatrix;
            asm volatile(
            "ldi r17, 8"          "\n"
            "L_sl2%=: "
            "ldd r2, %a[p]+1"     "\n\t"
            "mov r14, r2"         "\n\t"
            "ldd r3, %a[p]+2"     "\n\t"
            "ldd r4, %a[p]+3"     "\n\t"
            "ldd r5, %a[p]+4"     "\n\t"
            "ldd r6, %a[p]+5"     "\n\t"
            "ldd r7, %a[p]+6"     "\n\t"
            "ldd r8, %a[p]+7"     "\n\t"
            "ldd r9, %a[p]+8"     "\n\t"
            "ldd r10, %a[p]+9"    "\n\t"
            "ldd r11, %a[p]+10"   "\n\t"
            "ldd r12, %a[p]+11"   "\n\t"
            "ldd r13, %a[p]+0"    "\n\t"
            "ldi r16, 4"          "\n"
            "L_sl1%=: "
            "rol r14"             "\n\t"
            "rol r13"             "\n\t"
            "rol r12"             "\n\t"
            "rol r11"             "\n\t"
            "rol r10"             "\n\t"
            "rol r9"              "\n\t"
            "rol r8"              "\n\t"
            "rol r7"              "\n\t"
            "rol r6"              "\n\t"
            "rol r5"              "\n\t"
            "rol r4"              "\n\t"
            "rol r3"              "\n\t"
            "rol r2"              "\n\t"
            "dec r16"             "\n\t"
            "brne L_sl1%="        "\n\t"
            "std %a[p]+0, r2"     "\n\t"
            "std %a[p]+1, r3"     "\n\t"
            "std %a[p]+2, r4"     "\n\t"
            "std %a[p]+3, r5"     "\n\t"
            "std %a[p]+4, r6"     "\n\t"
            "std %a[p]+5, r7"     "\n\t"
            "std %a[p]+6, r8"     "\n\t"
            "std %a[p]+7, r9"     "\n\t"
            "std %a[p]+8, r10"    "\n\t"
            "std %a[p]+9, r11"    "\n\t"
            "std %a[p]+10, r12"   "\n\t"
            "std %a[p]+11, r13"   "\n\t"
            "adiw r30,12"         "\n\t"
            "dec r17"             "\n\t"
            "brne L_sl2%="        "\n\t"
            :
            : [p] "z" (p)
            : "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r16", "r17"
            );
        }
        break;

        case ScrollDown:
        {
            uint8_t *p = ledMatrix;
            asm volatile(
            "ldi r17, 8"          "\n"
            "L_sl2%=: "
            "ldd r2, %a[p]+11"    "\n\t"
            "ldd r3, %a[p]+0"     "\n\t"
            "ldd r4, %a[p]+1"     "\n\t"
            "ldd r5, %a[p]+2"     "\n\t"
            "ldd r6, %a[p]+3"     "\n\t"
            "ldd r7, %a[p]+4"     "\n\t"
            "ldd r8, %a[p]+5"     "\n\t"
            "ldd r9, %a[p]+6"     "\n\t"
            "ldd r10, %a[p]+7"    "\n\t"
            "ldd r11, %a[p]+8"    "\n\t"
            "ldd r12, %a[p]+9"    "\n\t"
            "ldd r13, %a[p]+10"   "\n\t"
            "mov r14, r13"        "\n\t"
            "ldi r16, 4"          "\n"
            "L_sl1%=: "
            "ror r14"             "\n\t"
            "ror r2"              "\n\t"
            "ror r3"              "\n\t"
            "ror r4"              "\n\t"
            "ror r5"              "\n\t"
            "ror r6"              "\n\t"
            "ror r7"              "\n\t"
            "ror r8"              "\n\t"
            "ror r9"              "\n\t"
            "ror r10"             "\n\t"
            "ror r11"             "\n\t"
            "ror r12"             "\n\t"
            "ror r13"             "\n\t"
            "dec r16"             "\n\t"
            "brne L_sl1%="        "\n\t"
            "std %a[p]+0, r2"     "\n\t"
            "std %a[p]+1, r3"     "\n\t"
            "std %a[p]+2, r4"     "\n\t"
            "std %a[p]+3, r5"     "\n\t"
            "std %a[p]+4, r6"     "\n\t"
            "std %a[p]+5, r7"     "\n\t"
            "std %a[p]+6, r8"     "\n\t"
            "std %a[p]+7, r9"     "\n\t"
            "std %a[p]+8, r10"    "\n\t"
            "std %a[p]+9, r11"    "\n\t"
            "std %a[p]+10, r12"   "\n\t"
            "std %a[p]+11, r13"   "\n\t"
            "adiw r30,12"         "\n\t"
            "dec r17"             "\n\t"
            "brne L_sl2%="        "\n\t"
            :
            : [p] "z" (p)
            : "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r16", "r17"
            );
        }
        break;
    }
}

    
void MeggyJr::fadePixel()
{
    // Fading the colors is way to slow in C++
    uint8_t *p = ledMatrix;
    asm volatile(
    "ldi r18, 96"         "\n"
    "L_sl2%=: "
    "ld r16, %a[p]"       "\n\t"
    "mov r17, r16"        "\n\t"
    "andi r17, 0x0f"      "\n\t"
    "breq L_n1%="         "\n\t"
    "dec r16"             "\n"
    "L_n1%=: "
    "mov r17, r16"        "\n\t"
    "andi r17, 0xf0"      "\n\t"
    "breq L_n2%="         "\n\t"
    "swap r16"            "\n\t"
    "dec r16"             "\n\t"
    "swap r16"            "\n\t"
    "L_n2%=: "
    "st %a[p]+, r16"      "\n\t"
    "dec r18"             "\n\t"
    "brne L_sl2%="        "\n\t"
    :
    : [p] "z" (p)
    : "r16", "r17", "r18"
    );
}


uint32_t MeggyJr::frameSync()
{
    const uint8_t lastValue = applicationFrameSync;
    while (applicationFrameSync == lastValue) {};
    return applicationFrame;
}
    
    
uint32_t MeggyJr::frameSyncShowLoad()
{
    if (applicationFrameMeasureState == ApplicationFrameMeasure_Ready) {
        const uint32_t frameTimeDelta = micros() - applicationFrameSyncLastTime;
        const uint16_t load = frameTimeDelta * 8 / applicationFrameDuration;
        if (load < 8) {
            setExtraLeds(((uint16_t)1 << load)-1);
        } else {
            uint8_t overLoad = load / 8;
            if (overLoad > 4) {
                overLoad = 4;
            }
            setExtraLeds(0xFF & ~(((uint16_t)1 << overLoad)-1));
        }
    } else {
        setExtraLeds(B00011000); // Wait for first real value.
    }
    const uint32_t frame = frameSync();
    applicationFrameSyncLastTime = micros();
    return frame;
}
    
    
bool MeggyJr::isAButtonPressed() const
{
    const uint8_t last = lastButtonState & ButtonA;
    const uint8_t current = currentButtonState & ButtonA;
    return last == 0 && last != current;
}

    
bool MeggyJr::isAButtonDown() const
{
    return (currentButtonState & ButtonA) != 0;
}

    
bool MeggyJr::isAButtonReleased() const
{
    const uint8_t last = lastButtonState & ButtonA;
    const uint8_t current = currentButtonState & ButtonA;
    return last != 0 && last != current;
}

    
bool MeggyJr::isBButtonPressed() const
{
    const uint8_t last = lastButtonState & ButtonB;
    const uint8_t current = currentButtonState & ButtonB;
    return last == 0 && last != current;
}

    
bool MeggyJr::isBButtonDown() const
{
    return (currentButtonState & ButtonB) != 0;
}

    
bool MeggyJr::isBButtonReleased() const
{
    const uint8_t last = lastButtonState & ButtonB;
    const uint8_t current = currentButtonState & ButtonB;
    return last != 0 && last != current;
}

    
bool MeggyJr::isUpButtonPressed() const
{
    const uint8_t last = lastButtonState & ButtonUp;
    const uint8_t current = currentButtonState & ButtonUp;
    return last == 0 && last != current;
}

    
bool MeggyJr::isUpButtonDown() const
{
    return (currentButtonState & ButtonUp) != 0;
}

    
bool MeggyJr::isUpButtonReleased() const
{
    const uint8_t last = lastButtonState & ButtonUp;
    const uint8_t current = currentButtonState & ButtonUp;
    return last != 0 && last != current;
}

    
bool MeggyJr::isDownButtonPressed() const
{
    const uint8_t last = lastButtonState & ButtonDown;
    const uint8_t current = currentButtonState & ButtonDown;
    return last == 0 && last != current;
}

    
bool MeggyJr::isDownButtonDown() const
{
    return (currentButtonState & ButtonDown) != 0;
}

    
bool MeggyJr::isDownButtonReleased() const
{
    const uint8_t last = lastButtonState & ButtonDown;
    const uint8_t current = currentButtonState & ButtonDown;
    return last != 0 && last != current;
}

    
bool MeggyJr::isLeftButtonPressed() const
{
    const uint8_t last = lastButtonState & ButtonLeft;
    const uint8_t current = currentButtonState & ButtonLeft;
    return last == 0 && last != current;
}

    
bool MeggyJr::isLeftButtonDown() const
{
    return (currentButtonState & ButtonLeft) != 0;
}

    
bool MeggyJr::isLeftButtonReleased() const
{
    const uint8_t last = lastButtonState & ButtonLeft;
    const uint8_t current = currentButtonState & ButtonLeft;
    return last != 0 && last != current;
}

    
bool MeggyJr::isRightButtonPressed() const
{
    const uint8_t last = lastButtonState & ButtonRight;
    const uint8_t current = currentButtonState & ButtonRight;
    return last == 0 && last != current;
}

    
bool MeggyJr::isRightButtonDown() const
{
    return (currentButtonState & ButtonRight) != 0;
}

    
bool MeggyJr::isRightButtonReleased() const
{
    const uint8_t last = lastButtonState & ButtonRight;
    const uint8_t current = currentButtonState & ButtonRight;
    return last != 0 && last != current;
}

    
uint8_t MeggyJr::getCurrentButtonState() const
{
    return currentButtonState;
}

    
uint8_t MeggyJr::getLastButtonState() const
{
    return lastButtonState;
}
    
    
void MeggyJr::playSound(const SoundToken *sound)
{
    cli();
    nextSoundToken = (const uint8_t*)sound;
    soundState = SoundNewRequest;
    sei();
}

    
void MeggyJr::stopSound()
{
    cli();
    soundState = SoundStopRequest;
    sei();
}

    
uint8_t MeggyJr::getPlayedNote() const
{
    uint8_t note;
    SoundState state;
    cli();
    note = currentNote;
    state = soundState;
    sei();
    if (state == SoundPlaying) {
        return note+1;
    } else {
        return 0;
    }
}

    
// The implementation of the Color class
// ----------------------------------------------------------------------------

    
// I use static functions for the default colors to
// keep the definitions in program memory.
#define COLOR_DEF(n, r, g, b) Color Color::n() { return Color(r, g, b); }
COLOR_DEF(black, 0, 0, 0);
COLOR_DEF(red, 12, 0, 0);
COLOR_DEF(orange, 12, 1, 0);
COLOR_DEF(yellow, 10, 4, 0);
COLOR_DEF(green, 0, 5, 0);
COLOR_DEF(blue, 0, 0, 5);
COLOR_DEF(violet, 8, 0, 4);
COLOR_DEF(white, 14, 4, 2);
COLOR_DEF(darkRed, 4, 0, 0);
COLOR_DEF(darkOrange, 7, 1, 0);
COLOR_DEF(darkYellow, 4, 1, 0);
COLOR_DEF(darkGreen, 0, 1, 0);
COLOR_DEF(darkBlue, 0, 0, 1);
COLOR_DEF(darkViolet, 4, 0, 2);
COLOR_DEF(gray, 7, 2, 1);
COLOR_DEF(maximum, 15, 15, 15);

    
Color::Color(uint8_t red, uint8_t green, uint8_t blue)
{
    _color = red << 8 | green << 4 | blue;
}

    
uint8_t Color::getRed() const
{
    return _color >> 8;
}

    
uint8_t Color::getGreen() const
{
    return (_color >> 4) & 0x0F;
}

    
uint8_t Color::getBlue() const
{
    return (_color & 0x0F);
}

    
}

// End of File
// ----------------------------------------------------------------------------
//
