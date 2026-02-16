#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

namespace OP1Colors
{
    // OP-1 Field characteristic colors
    const juce::Colour background      { 0xFF1A1A2E };  // Deep navy/black background
    const juce::Colour displayBg       { 0xFF0D0D1A };  // AMOLED-style display black
    const juce::Colour displayFrame    { 0xFF2A2A3E };  // Display bezel
    const juce::Colour bodyColor       { 0xFFE8E4DF };  // White/cream aluminum body
    const juce::Colour bodyDark        { 0xFFD0CCC7 };  // Darker body shade

    // OP-1 Field accent colors (the iconic colored encoders)
    const juce::Colour encoderBlue     { 0xFF00B4D8 };  // Blue encoder
    const juce::Colour encoderGreen    { 0xFF2DC653 };  // Green encoder
    const juce::Colour encoderWhite    { 0xFFEEEEEE };  // White encoder
    const juce::Colour encoderOrange   { 0xFFFF6B35 };  // Orange encoder

    // Display text/element colors
    const juce::Colour textPrimary     { 0xFFFFFFFF };  // White text
    const juce::Colour textSecondary   { 0xFF8888AA };  // Muted text
    const juce::Colour textDim         { 0xFF444466 };  // Very dim text

    // Mode colors matching OP-1 Field screens
    const juce::Colour synthColor      { 0xFF00C8FF };  // Synth mode - cyan/blue
    const juce::Colour drumColor       { 0xFFFF4444 };  // Drum mode - red
    const juce::Colour tapeColor       { 0xFF44FF44 };  // Tape mode - green
    const juce::Colour mixerColor      { 0xFFFFAA00 };  // Mixer mode - orange/yellow
    const juce::Colour sequencerColor  { 0xFFFF44FF };  // Sequencer - magenta

    // Waveform / visualization colors
    const juce::Colour waveformBlue    { 0xFF4488FF };
    const juce::Colour waveformGreen   { 0xFF44FF88 };
    const juce::Colour waveformRed     { 0xFFFF4488 };
    const juce::Colour waveformYellow  { 0xFFFFCC44 };

    // Track colors (tape)
    const juce::Colour track1          { 0xFF4488FF };
    const juce::Colour track2          { 0xFFFF6644 };
    const juce::Colour track3          { 0xFF44FF88 };
    const juce::Colour track4          { 0xFFFFCC44 };

    // Key colors
    const juce::Colour keyWhite        { 0xFFE0DCD8 };
    const juce::Colour keyBlack        { 0xFF2A2A3E };
    const juce::Colour keyPressed      { 0xFF00B4D8 };

    // VU Meter
    const juce::Colour vuGreen         { 0xFF22CC44 };
    const juce::Colour vuYellow        { 0xFFCCCC22 };
    const juce::Colour vuRed           { 0xFFCC2222 };
}
