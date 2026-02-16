#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

namespace OP1Colors
{
    // ─── Body / Hardware ────────────────────────────────────────────
    // The OP-1 Field body is a matte white/silver aluminum
    const juce::Colour body             { 0xFFEAE7E2 };
    const juce::Colour bodyHighlight    { 0xFFF5F3F0 };
    const juce::Colour bodyShadow      { 0xFFCCC8C3 };
    const juce::Colour bodyEdge        { 0xFFD8D5D0 };
    const juce::Colour screwColor      { 0xFFB8B4AE };

    // ─── Display ────────────────────────────────────────────────────
    // True AMOLED black with subtle warmth
    const juce::Colour displayBg       { 0xFF000000 };
    const juce::Colour displayBezel    { 0xFF1C1C1C };
    const juce::Colour displayBorder   { 0xFF333333 };

    // ─── Encoder Colors (exact OP-1 Field hues) ────────────────────
    const juce::Colour encoderBlue     { 0xFF00AADD };
    const juce::Colour encoderGreen    { 0xFF00CC66 };
    const juce::Colour encoderWhite    { 0xFFE8E8E8 };
    const juce::Colour encoderOrange   { 0xFFFF6633 };

    // ─── Typography on display ──────────────────────────────────────
    const juce::Colour textBright      { 0xFFFFFFFF };
    const juce::Colour textPrimary     { 0xFFDDDDDD };
    const juce::Colour textSecondary   { 0xFF888888 };
    const juce::Colour textDim         { 0xFF444444 };
    const juce::Colour textMuted       { 0xFF2A2A2A };

    // ─── Mode accent colors (OP-1 Field screen palette) ────────────
    // These are the exact signature colors from the OP-1 Field UI
    const juce::Colour synthColor      { 0xFF00BBFF };  // Bright cyan-blue
    const juce::Colour drumColor       { 0xFFFF3355 };  // Warm red-pink
    const juce::Colour tapeColor       { 0xFF33DD77 };  // Fresh green
    const juce::Colour mixerColor      { 0xFFFFBB22 };  // Warm amber
    const juce::Colour sequencerColor  { 0xFFCC55FF };  // Purple-magenta

    // ─── Waveform & visualization ───────────────────────────────────
    const juce::Colour waveBlue        { 0xFF3399FF };
    const juce::Colour waveGreen       { 0xFF33FFAA };
    const juce::Colour waveRed         { 0xFFFF5566 };
    const juce::Colour waveYellow      { 0xFFFFDD44 };
    const juce::Colour wavePurple      { 0xFFBB66FF };
    const juce::Colour waveCyan        { 0xFF44EEFF };

    // ─── Tape track colors ──────────────────────────────────────────
    const juce::Colour track1          { 0xFF3399FF };
    const juce::Colour track2          { 0xFFFF6644 };
    const juce::Colour track3          { 0xFF33DDAA };
    const juce::Colour track4          { 0xFFFFCC33 };

    // ─── Keyboard ───────────────────────────────────────────────────
    const juce::Colour keyWhite        { 0xFFE2DED9 };
    const juce::Colour keyWhiteEdge    { 0xFFD4D0CB };
    const juce::Colour keyBlack        { 0xFF3A3A3A };
    const juce::Colour keyBlackEdge    { 0xFF2A2A2A };
    const juce::Colour keyActive       { 0xFF00AADD };

    // ─── VU / Metering ──────────────────────────────────────────────
    const juce::Colour vuGreen         { 0xFF00DD55 };
    const juce::Colour vuYellow        { 0xFFDDDD00 };
    const juce::Colour vuRed           { 0xFFDD2222 };

    // ─── Buttons ────────────────────────────────────────────────────
    const juce::Colour buttonFace      { 0xFFD8D4CF };
    const juce::Colour buttonEdge      { 0xFFC4C0BB };
    const juce::Colour buttonActive    { 0xFF00AADD };
    const juce::Colour buttonText      { 0xFF555555 };
    const juce::Colour buttonTextLight { 0xFFFFFFFF };
}
