#include "DrumScreen.h"
#include "../LookAndFeel/OP1LookAndFeel.h"

void DrumScreen::setSlotLevel(int slot, float level)
{
    if (slot >= 0 && slot < 8)
        slotLevels[slot] = level;
}

void DrumScreen::triggerSlot(int slot)
{
    if (slot >= 0 && slot < 8)
        slotTriggers[slot] = 1.0f;
}

void DrumScreen::paint(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Pad colors cycling through the specified palette
    const juce::Colour padColors[] = {
        OP1Colors::waveBlue,      OP1Colors::waveRed,
        OP1Colors::waveYellow,    OP1Colors::waveGreen,
        OP1Colors::encoderOrange, OP1Colors::synthColor,
        OP1Colors::waveRed,       OP1Colors::waveYellow
    };

    int slotWidth = bounds.getWidth() / 8;

    for (int i = 0; i < 8; ++i)
    {
        auto slotBounds = juce::Rectangle<int>(
            bounds.getX() + i * slotWidth, bounds.getY(),
            slotWidth, bounds.getHeight());

        auto labelArea = slotBounds.removeFromBottom(16);
        auto padArea = slotBounds.reduced(3, 6);
        auto padRect = padArea.toFloat();

        bool isSelected = (i == activeSlot);
        float trigger = slotTriggers[i];
        juce::Colour padColor = padColors[i];

        // --- Pad fill ---
        // Inactive: background at 5% alpha of pad color
        // Triggered: fill at 40% alpha, fading out
        float fillAlpha = 0.05f + trigger * 0.40f;
        g.setColour(padColor.withAlpha(fillAlpha));
        g.fillRoundedRectangle(padRect, 4.0f);

        // --- Pad border ---
        if (isSelected)
        {
            // Active/selected: 1.5px border at full color
            g.setColour(padColor);
            g.drawRoundedRectangle(padRect, 4.0f, 1.5f);
        }
        else
        {
            // Inactive: 1px border at 30% alpha
            g.setColour(padColor.withAlpha(0.30f));
            g.drawRoundedRectangle(padRect, 4.0f, 1.0f);
        }

        // --- Drum name label ---
        g.setColour(OP1Colors::textSecondary);
        g.setFont(OP1LookAndFeel::getDisplayFont(8.0f));
        g.drawText(drumNames[i], labelArea, juce::Justification::centred);

        // Decay trigger animation (multiply by 0.93f per frame)
        slotTriggers[i] *= 0.93f;
    }
}
