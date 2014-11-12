#pragma once
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


#include "Arduino.h"


// Include the own definitions.
#include "LRColor.h"
#include "LRSoundToken.h"


namespace lr {


/// The interface to access the Meggy Jr.
///
/// All methods which end in a upper case "S" (for safe) will check the
/// input parameter. If they are out of range, "set" calls are
/// just ignored, and "get" calls return a default value. If no
/// "S" version of a method exists, the method will behave like
/// a safe method.
///
/// Methods not ending in "S" will have an unexpected behaviour
/// if the parameters are not in a valid range, but they work faster.
///
class MeggyJr
{
public:
    /// The frame rate for the application.
    enum FrameRate : uint8_t {
        FrameRate120 = 1, // 120 frames per second.
        FrameRate60  = 2, // 60 frames per second.
        FrameRate30  = 4, // 30 frames per second.
        FrameRate15  = 8, // 15 frames per second.
    };
    
    /// Button masks.
    enum ButtonMask : uint8_t {
        ButtonA     = B00000010,
        ButtonB     = B00000001,
        ButtonUp    = B00000100,
        ButtonDown  = B00001000,
        ButtonLeft  = B00010000,
        ButtonRight = B00100000
    };
    
    /// Scroll directions.
    enum ScrollDirection : uint8_t {
        ScrollUp,
        ScrollDown,
        ScrollLeft,
        ScrollRight
    };
    
public:
    /// The setup method to initialize the interface.
    void setup(FrameRate frameRate = FrameRate30);
    
    /// Clear the display and extra LEDs. All LEDs go black.
    void clear();

    
    // --- Matrix LED Methods ---
    
    /// Clear all pixels.
    ///
    /// This will just clear all pixels in the 8x8 matrix
    /// and set them to black.
    ///
    void clearPixels();
    
    /// Set the color of a pixel.
    ///
    /// @param x The x position of the pixel (0-7).
    /// @param y The y position of the pixel (0-7).
    /// @param color The color for the pixel.
    ///
    void setPixel(uint8_t x, uint8_t y, const Color &color);
    void setPixelS(uint8_t x, uint8_t y, const Color &color);
    
    /// Get the color from a pixel.
    ///
    /// @param x The x position of the pixel (0-7).
    /// @param y The y position of the pixel (0-7).
    /// @return The color of the pixel.
    ///
    Color getPixel(uint8_t x, uint8_t y) const;
    Color getPixelS(uint8_t x, uint8_t y) const;
    
    /// Fill a rectangle with a given color
    ///
    void fillRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const Color &color);
    void fillRectS(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const Color &color);
    
    /// Scroll the current matrix in the given direction.
    ///
    /// Pixels are wraped around.
    ///
    void scrollPixel(ScrollDirection scrollDirection);
    
    /// Fade pixels to black.
    ///
    void fadePixel();
    
    // --- Synchronization --
    
    /// Wait for the display synchronization.
    ///
    /// This method waits for the display to synchronize and
    /// returns the frame number. The frame rate depends on the
    /// value you passed to the setup() method.
    ///
    /// After this method, you have time for your logic
    /// to react to buttons and to draw the next frame to the
    /// display using all the pixel methods.
    ///
    uint32_t frameSync();
    
    /// Wait for the display synchronization and show the load.
    ///
    /// This special version of the display synchronization works
    /// similar to the frameSync() method, but uses the extra LEDs
    /// on top of the display to visualize how much time your
    /// code uses, relatively to the frame rate. 0 LEDs = 0% 8 LEDs = 100% (critical).
    ///
    /// If the load is over 100%, the leds from the left side are disabled for each skipped
    /// frame, up to 4 skipped frames.
    ///
    /// Obviously, this method should only be used for testing.
    ///
    uint32_t frameSyncShowLoad();
    
    
    // --- Extra LED Methods ---
    
    /// Set the bits for the extra LEDs.
    ///
    /// @param bits A bit mask to set all LEDs at once.
    ///
    void setExtraLeds(uint8_t bits);

    /// Get the bitmask for the extra LEDs.
    ///
    /// @return A bitmask with the status of all extra LEDs.
    ///
    uint8_t getExtraLeds() const;
    
    /// Enable an extra LED
    ///
    /// @param index The index of the LED from 0 to 7.
    ///
    void enableExtraLed(uint8_t index);

    /// Diable an extra LED
    ///
    /// @param index The index of the LED from 0 to 7.
    ///
    void disableExtraLed(uint8_t index);
    
    /// Check the status of an extra LED
    ///
    /// @param index The index of the LED from 0 to 7.
    /// @return true if the LED is enabled, false otherwise.
    ///
    bool isExtraLedEnabled(uint8_t index) const;
    
    
    // --- Buttons ---
    
    /// Check if the A button was pressed.
    ///
    bool isAButtonPressed() const;
    
    /// Check if currently the A button is pressed down.
    ///
    bool isAButtonDown() const;
    
    /// Check if the A button was released.
    ///
    bool isAButtonReleased() const;

    /// Check if the B button was pressed.
    ///
    bool isBButtonPressed() const;
    
    /// Check if currently the B button is pressed down.
    ///
    bool isBButtonDown() const;
    
    /// Check if the B button was released.
    ///
    bool isBButtonReleased() const;
    
    /// Check if the up button was pressed.
    ///
    bool isUpButtonPressed() const;
    
    /// Check if currently the up button is pressed down.
    ///
    bool isUpButtonDown() const;
    
    /// Check if the up button was released.
    ///
    bool isUpButtonReleased() const;
    
    /// Check if the down button was pressed.
    ///
    bool isDownButtonPressed() const;
    
    /// Check if currently the down button is pressed down.
    ///
    bool isDownButtonDown() const;
    
    /// Check if the down button was released.
    ///
    bool isDownButtonReleased() const;

    /// Check if the left button was pressed.
    ///
    bool isLeftButtonPressed() const;
    
    /// Check if currently the left button is pressed down.
    ///
    bool isLeftButtonDown() const;
    
    /// Check if the left button was released.
    ///
    bool isLeftButtonReleased() const;

    /// Check if the right button was pressed.
    ///
    bool isRightButtonPressed() const;
    
    /// Check if currently the right button is pressed down.
    ///
    bool isRightButtonDown() const;
    
    /// Check if the right button was released.
    ///
    bool isRightButtonReleased() const;
    
    /// Get the current button state.
    ///
    /// You can mask the bits using the ButtonMask enumeration values.
    ///
    uint8_t getCurrentButtonState() const;
    
    /// Get the last button state.
    ///
    /// You can mask the bits using the ButtonMask enumeration values.
    ///
    uint8_t getLastButtonState() const;
    
    
    // --- Sound ---
    
    /// Play the given sound.
    ///
    /// You have to define the sound or melody in the program memory!
    ///
    void playSound(const SoundToken* sound);
    
    /// Stop any playing sound immediately.
    ///
    void stopSound();
    
    /// Get the current note which is playing.
    ///
    /// This will be a note from the SoundToken enumeration, or 0
    /// if no note is playing.
    ///
    uint8_t getPlayedNote() const;
};

    
extern MeggyJr meg;
    
    
}

