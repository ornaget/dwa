#include "MixerScreen.h"
#include "../LookAndFeel/OP1LookAndFeel.h"

void MixerScreen::setTrackVolume(int track, float volume)
{
    if (track >= 0 && track < 4) trackVolumes[track] = volume;
}

void MixerScreen::setTrackLevel(int track, float level)
{
    if (track >= 0 && track < 4) trackLevels[track] = level;
}

void MixerScreen::paint(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    juce::Colour trackColors[] = { OP1Colors::track1, OP1Colors::track2,
                                    OP1Colors::track3, OP1Colors::track4 };

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

    // EQ display
    drawEQ(g, bounds);
}

void MixerScreen::drawChannel(juce::Graphics& g, juce::Rectangle<int> bounds, int track, juce::Colour color)
{
    bounds.reduce(2, 2);
    auto labelArea = bounds.removeFromBottom(14);
    auto faderArea = bounds;

    // Label
    g.setColour(color);
    g.setFont(OP1LookAndFeel::getOP1Font(10.0f));
    g.drawText("T" + juce::String(track + 1), labelArea, juce::Justification::centred);

    // Fader track
    auto faderTrack = faderArea.reduced(faderArea.getWidth() / 3, 2);

    g.setColour(OP1Colors::textDim);
    g.fillRoundedRectangle(faderTrack.toFloat(), 2.0f);

    // Level meter
    float level = trackLevels[track];
    auto levelBar = faderTrack.toFloat();
    float barHeight = levelBar.getHeight() * level;
    levelBar.setY(levelBar.getBottom() - barHeight);
    levelBar.setHeight(barHeight);
    g.setColour(color.withAlpha(0.4f));
    g.fillRoundedRectangle(levelBar, 2.0f);

    // Fader position
    float faderY = faderTrack.getY() + (1.0f - trackVolumes[track]) * (float)faderTrack.getHeight();
    auto faderHandle = juce::Rectangle<float>(
        (float)faderTrack.getX() - 3.0f, faderY - 3.0f,
        (float)faderTrack.getWidth() + 6.0f, 6.0f);

    g.setColour(color);
    g.fillRoundedRectangle(faderHandle, 2.0f);
}

void MixerScreen::drawMaster(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    bounds.reduce(2, 2);
    auto labelArea = bounds.removeFromBottom(14);
    auto faderArea = bounds;

    // Label
    g.setColour(OP1Colors::textPrimary);
    g.setFont(OP1LookAndFeel::getOP1BoldFont(10.0f));
    g.drawText("MST", labelArea, juce::Justification::centred);

    // Master fader
    auto faderTrack = faderArea.reduced(faderArea.getWidth() / 3, 2);

    g.setColour(OP1Colors::textDim);
    g.fillRoundedRectangle(faderTrack.toFloat(), 2.0f);

    float faderY = faderTrack.getY() + (1.0f - masterVolume) * (float)faderTrack.getHeight();
    auto faderHandle = juce::Rectangle<float>(
        (float)faderTrack.getX() - 3.0f, faderY - 3.0f,
        (float)faderTrack.getWidth() + 6.0f, 6.0f);

    g.setColour(OP1Colors::textPrimary);
    g.fillRoundedRectangle(faderHandle, 2.0f);

    // Drive indicator
    if (drive > 0.01f)
    {
        g.setColour(OP1Colors::vuRed.withAlpha(drive));
        float driveH = (float)faderTrack.getHeight() * drive;
        auto driveBar = faderTrack.toFloat();
        driveBar.setY(driveBar.getBottom() - driveH);
        driveBar.setHeight(driveH);
        g.fillRoundedRectangle(driveBar, 2.0f);
    }
}

void MixerScreen::drawEQ(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    bounds.reduce(4, 4);

    g.setColour(OP1Colors::textDim);
    g.setFont(OP1LookAndFeel::getOP1Font(9.0f));
    g.drawText("EQ", bounds.removeFromTop(12), juce::Justification::centred);

    // EQ curve
    float centerY = (float)bounds.getCentreY();
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
        float midWidth = 0.2f;
        y += eqMid * std::exp(-std::pow((t - midCenter) / midWidth, 2.0f));
        // High shelf
        y += eqHigh * std::exp(-(1.0f - t) * 5.0f);

        eqPath.lineTo((float)(bounds.getX() + x), centerY - y * height);
    }

    // Center line
    g.setColour(OP1Colors::textDim.withAlpha(0.3f));
    g.drawHorizontalLine((int)centerY, (float)bounds.getX(), (float)bounds.getRight());

    g.setColour(OP1Colors::mixerColor);
    g.strokePath(eqPath, juce::PathStrokeType(1.5f));
}
