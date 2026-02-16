#include "MixerScreen.h"
#include "../LookAndFeel/OP1LookAndFeel.h"

void MixerScreen::setTrackVolume(int track, float volume)
{
    if (track >= 0 && track < 4)
        trackVolumes[track] = volume;
}

void MixerScreen::setTrackLevel(int track, float level)
{
    if (track >= 0 && track < 4)
        trackLevels[track] = level;
}

void MixerScreen::paint(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    const juce::Colour trackColors[] = {
        OP1Colors::track1, OP1Colors::track2,
        OP1Colors::track3, OP1Colors::track4
    };

    int channelWidth = bounds.getWidth() / 6;

    // 4 track channels
    for (int t = 0; t < 4; ++t)
    {
        auto chBounds = bounds.removeFromLeft(channelWidth);
        drawChannel(g, chBounds, t, trackColors[t]);
    }

    // Master channel
    auto masterBounds = bounds.removeFromLeft(channelWidth);
    drawMaster(g, masterBounds);

    // EQ display fills remaining space
    drawEQ(g, bounds);
}

void MixerScreen::drawChannel(juce::Graphics& g, juce::Rectangle<int> bounds,
                               int track, juce::Colour color)
{
    bounds.reduce(4, 4);
    auto labelArea = bounds.removeFromBottom(14);
    auto faderArea = bounds;

    // --- Track label ---
    g.setColour(color);
    g.setFont(OP1LookAndFeel::getDisplayFont(10.0f));
    g.drawText("T" + juce::String(track + 1), labelArea, juce::Justification::centred);

    // --- Fader track: 3px wide rounded rectangle in textMuted, centered ---
    float trackCenterX = (float)faderArea.getCentreX();
    auto faderTrack = juce::Rectangle<float>(
        trackCenterX - 1.5f, (float)faderArea.getY() + 2.0f,
        3.0f, (float)faderArea.getHeight() - 4.0f);

    g.setColour(OP1Colors::textMuted);
    g.fillRoundedRectangle(faderTrack, 1.5f);

    // --- Level meter behind fader: track color at 25% alpha ---
    float level = trackLevels[track];
    if (level > 0.01f)
    {
        auto levelBar = faderTrack;
        float barHeight = levelBar.getHeight() * level;
        levelBar.setY(levelBar.getBottom() - barHeight);
        levelBar.setHeight(barHeight);
        g.setColour(color.withAlpha(0.25f));
        g.fillRoundedRectangle(levelBar, 1.5f);
    }

    // --- Fader handle: 8px wide, 4px tall rounded rect in track color ---
    float faderY = faderTrack.getY()
        + (1.0f - trackVolumes[track]) * faderTrack.getHeight();
    auto faderHandle = juce::Rectangle<float>(
        trackCenterX - 4.0f, faderY - 2.0f, 8.0f, 4.0f);

    g.setColour(color);
    g.fillRoundedRectangle(faderHandle, 1.5f);
}

void MixerScreen::drawMaster(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    bounds.reduce(4, 4);
    auto labelArea = bounds.removeFromBottom(14);
    auto faderArea = bounds;

    // --- Master label ---
    g.setColour(OP1Colors::textBright);
    g.setFont(OP1LookAndFeel::getDisplayBoldFont(10.0f));
    g.drawText("MST", labelArea, juce::Justification::centred);

    // --- Fader track: 3px wide rounded rectangle in textMuted ---
    float trackCenterX = (float)faderArea.getCentreX();
    auto faderTrack = juce::Rectangle<float>(
        trackCenterX - 1.5f, (float)faderArea.getY() + 2.0f,
        3.0f, (float)faderArea.getHeight() - 4.0f);

    g.setColour(OP1Colors::textMuted);
    g.fillRoundedRectangle(faderTrack, 1.5f);

    // --- Drive indicator: vuRed glow on master fader area ---
    if (drive > 0.01f)
    {
        g.setColour(OP1Colors::vuRed.withAlpha(drive * 0.35f));
        float driveHeight = faderTrack.getHeight() * drive;
        auto driveBar = faderTrack;
        driveBar.setY(driveBar.getBottom() - driveHeight);
        driveBar.setHeight(driveHeight);
        g.fillRoundedRectangle(driveBar.expanded(2.0f, 0.0f), 2.0f);
    }

    // --- Fader handle: 8px wide, 4px tall, textBright for master ---
    float faderY = faderTrack.getY()
        + (1.0f - masterVolume) * faderTrack.getHeight();
    auto faderHandle = juce::Rectangle<float>(
        trackCenterX - 4.0f, faderY - 2.0f, 8.0f, 4.0f);

    g.setColour(OP1Colors::textBright);
    g.fillRoundedRectangle(faderHandle, 1.5f);
}

void MixerScreen::drawEQ(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    bounds.reduce(6, 4);

    // EQ label
    g.setColour(OP1Colors::textDim);
    g.setFont(OP1LookAndFeel::getDisplayFont(9.0f));
    g.drawText("EQ", bounds.removeFromTop(12), juce::Justification::centred);

    // Center line in textMuted at 20% alpha
    float centerY = (float)bounds.getCentreY();
    g.setColour(OP1Colors::textMuted.withAlpha(0.20f));
    g.drawLine((float)bounds.getX(), centerY,
               (float)bounds.getRight(), centerY, 0.5f);

    // EQ curve: smooth path in mixerColor, 1.5px stroke
    float height = (float)bounds.getHeight() * 0.4f;

    juce::Path eqPath;
    eqPath.startNewSubPath((float)bounds.getX(), centerY);

    for (int x = 0; x < bounds.getWidth(); ++x)
    {
        float t = (float)x / (float)bounds.getWidth();
        float y = 0.0f;

        // Low shelf
        y += eqLow * std::exp(-t * 5.0f);
        // Mid bell
        float midCenter = 0.5f;
        float midWidth  = 0.2f;
        y += eqMid * std::exp(-std::pow((t - midCenter) / midWidth, 2.0f));
        // High shelf
        y += eqHigh * std::exp(-(1.0f - t) * 5.0f);

        eqPath.lineTo((float)(bounds.getX() + x), centerY - y * height);
    }

    g.setColour(OP1Colors::mixerColor);
    g.strokePath(eqPath, juce::PathStrokeType(1.5f,
        juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}
