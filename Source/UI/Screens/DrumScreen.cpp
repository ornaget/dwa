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
    int slotWidth = bounds.getWidth() / 8;

    juce::Colour slotColors[] = {
        OP1Colors::waveformBlue, OP1Colors::waveformRed,
        OP1Colors::waveformYellow, OP1Colors::waveformGreen,
        OP1Colors::encoderOrange, OP1Colors::encoderBlue,
        OP1Colors::waveformRed, OP1Colors::waveformYellow
    };

    for (int i = 0; i < 8; ++i)
    {
        auto slotBounds = juce::Rectangle<int>(
            bounds.getX() + i * slotWidth, bounds.getY(),
            slotWidth, bounds.getHeight());

        auto labelArea = slotBounds.removeFromBottom(16);
        auto padArea = slotBounds.reduced(4, 8);

        // Drum pad visualization
        bool isActive = (i == activeSlot);
        float trigger = slotTriggers[i];

        // Pad background
        g.setColour(slotColors[i].withAlpha(0.1f + trigger * 0.4f));
        g.fillRoundedRectangle(padArea.toFloat(), 4.0f);

        // Pad border
        g.setColour(isActive ? slotColors[i] : slotColors[i].withAlpha(0.3f));
        g.drawRoundedRectangle(padArea.toFloat(), 4.0f, isActive ? 2.0f : 1.0f);

        // Level visualization (bar inside pad)
        if (slotLevels[i] > 0.01f || trigger > 0.01f)
        {
            float level = juce::jmax(slotLevels[i], trigger);
            auto levelBar = padArea.toFloat();
            float barHeight = levelBar.getHeight() * level;
            levelBar.setY(levelBar.getBottom() - barHeight);
            levelBar.setHeight(barHeight);

            g.setColour(slotColors[i].withAlpha(0.5f));
            g.fillRoundedRectangle(levelBar.reduced(2.0f), 2.0f);
        }

        // Waveform icon inside pad
        if (trigger < 0.3f)
        {
            juce::Path miniWave;
            float cx = padArea.getCentreX();
            float cy = (float)padArea.getCentreY();
            float amp = (float)padArea.getHeight() * 0.15f;

            miniWave.startNewSubPath(cx - 12, cy);
            for (int px = -12; px <= 12; ++px)
            {
                float t = (float)(px + 12) / 24.0f;
                float wave = std::sin(t * juce::MathConstants<float>::twoPi * 2.0f) * amp;
                miniWave.lineTo(cx + (float)px, cy + wave);
            }

            g.setColour(slotColors[i].withAlpha(0.5f));
            g.strokePath(miniWave, juce::PathStrokeType(1.0f));
        }

        // Label
        g.setColour(isActive ? OP1Colors::textPrimary : OP1Colors::textSecondary);
        g.setFont(OP1LookAndFeel::getOP1Font(9.0f));
        g.drawText(drumNames[i], labelArea, juce::Justification::centred);

        // Decay triggers
        slotTriggers[i] *= 0.92f;
    }
}
