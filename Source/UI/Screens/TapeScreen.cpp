#include "TapeScreen.h"
#include "../LookAndFeel/OP1LookAndFeel.h"

void TapeScreen::setTrackData(int track, const std::vector<float>& data)
{
    if (track >= 0 && track < 4)
        trackWaveforms[track] = data;
}

void TapeScreen::paint(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Left third: tape reels. Right two-thirds: track lanes + transport bar
    auto reelArea = bounds.removeFromLeft(bounds.getWidth() / 3);
    auto infoArea = bounds.removeFromBottom(20);
    auto trackArea = bounds;

    drawTapeReels(g, reelArea);
    drawTracks(g, trackArea);
    drawPlayhead(g, trackArea);
    drawTransportInfo(g, infoArea);
}

void TapeScreen::drawTapeReels(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    float reelSize = juce::jmin((float)bounds.getWidth() * 0.32f,
                                 (float)bounds.getHeight() * 0.35f);

    // Left reel (supply) - positioned in upper-left area
    float lx = (float)bounds.getX() + (float)bounds.getWidth() * 0.30f;
    float ly = (float)bounds.getCentreY() - reelSize * 0.25f;

    // Right reel (take-up) - positioned in upper-right area
    float rx = (float)bounds.getX() + (float)bounds.getWidth() * 0.70f;
    float ry = ly;

    // Reel sizes vary with tape position
    float leftReelSize  = reelSize * (0.5f + (1.0f - playPosition) * 0.5f);
    float rightReelSize = reelSize * (0.5f + playPosition * 0.5f);

    // Advance reel angle when playing
    if (isPlaying)
        tapeReelAngle += 0.05f;

    for (int reel = 0; reel < 2; ++reel)
    {
        float cx   = reel == 0 ? lx : rx;
        float cy   = reel == 0 ? ly : ry;
        float size = reel == 0 ? leftReelSize : rightReelSize;

        // Reel circle: 1px stroke in textDim
        g.setColour(OP1Colors::textDim);
        g.drawEllipse(cx - size, cy - size, size * 2.0f, size * 2.0f, 1.0f);

        // Hub dot in textSecondary
        g.setColour(OP1Colors::textSecondary);
        g.fillEllipse(cx - 3.0f, cy - 3.0f, 6.0f, 6.0f);

        // 3 animated spokes: thin 0.5px lines
        float direction = reel == 0 ? -1.0f : 1.0f;
        g.setColour(OP1Colors::textDim.withAlpha(0.5f));
        for (int spoke = 0; spoke < 3; ++spoke)
        {
            float angle = tapeReelAngle * direction
                + (float)spoke * juce::MathConstants<float>::twoPi / 3.0f;
            float sx = cx + std::cos(angle) * size * 0.8f;
            float sy = cy + std::sin(angle) * size * 0.8f;
            g.drawLine(cx, cy, sx, sy, 0.5f);
        }
    }

    // Tape line connecting reels: textDim at 30% alpha
    g.setColour(OP1Colors::textDim.withAlpha(0.30f));
    g.drawLine(lx + leftReelSize, ly, rx - rightReelSize, ry, 0.5f);
}

void TapeScreen::drawTracks(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    const juce::Colour trackColors[] = {
        OP1Colors::track1, OP1Colors::track2,
        OP1Colors::track3, OP1Colors::track4
    };

    int trackHeight = bounds.getHeight() / 4;

    for (int t = 0; t < 4; ++t)
    {
        auto trackBounds = juce::Rectangle<int>(
            bounds.getX(), bounds.getY() + t * trackHeight,
            bounds.getWidth(), trackHeight);

        bool isActive = (t == activeTrack);

        // Active track: background at track color 8% alpha
        if (isActive)
        {
            g.setColour(trackColors[t].withAlpha(0.08f));
            g.fillRect(trackBounds);
        }

        // Track number label
        auto labelArea = trackBounds.removeFromLeft(14);
        if (isActive)
            g.setColour(trackColors[t]);
        else
            g.setColour(trackColors[t].withAlpha(0.25f));

        g.setFont(OP1LookAndFeel::getDisplayFont(9.0f));
        g.drawText(juce::String(t + 1), labelArea, juce::Justification::centred);

        // Track waveform or empty center line
        if (!trackWaveforms[t].empty())
        {
            juce::Path wavePath;
            float cy = (float)trackBounds.getCentreY();
            float amp = (float)trackHeight * 0.35f;
            int dataSize = (int)trackWaveforms[t].size();

            wavePath.startNewSubPath((float)trackBounds.getX(), cy);
            for (int x = 0; x < trackBounds.getWidth(); ++x)
            {
                float viewStart = playPosition - 0.1f;
                float viewEnd   = playPosition + 0.1f;
                float tPos = viewStart + (float)x / (float)trackBounds.getWidth()
                             * (viewEnd - viewStart);
                int idx = (int)(tPos * (float)dataSize);
                idx = juce::jlimit(0, dataSize - 1, idx);
                float y = cy - trackWaveforms[t][(size_t)idx] * amp;
                wavePath.lineTo((float)(trackBounds.getX() + x), y);
            }

            g.setColour(trackColors[t].withAlpha(isActive ? 0.8f : 0.3f));
            g.strokePath(wavePath, juce::PathStrokeType(1.0f,
                juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }
        else
        {
            // Empty track: center line at track color 5% alpha
            g.setColour(trackColors[t].withAlpha(0.05f));
            g.drawLine((float)trackBounds.getX(), (float)trackBounds.getCentreY(),
                       (float)trackBounds.getRight(), (float)trackBounds.getCentreY(), 0.5f);
        }
    }
}

void TapeScreen::drawPlayhead(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Playhead: vertical line at center, 1px
    float x = (float)bounds.getCentreX();

    // textBright when playing, vuRed when recording
    if (isRecording)
        g.setColour(OP1Colors::vuRed);
    else if (isPlaying)
        g.setColour(OP1Colors::textBright);
    else
        g.setColour(OP1Colors::textDim);

    g.drawLine(x, (float)bounds.getY(), x, (float)bounds.getBottom(), 1.0f);
}

void TapeScreen::drawTransportInfo(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    bounds.reduce(4, 0);
    g.setFont(OP1LookAndFeel::getDisplayFont(10.0f));

    // Time code: position mapped to 6-minute tape
    float seconds = playPosition * 360.0f;
    int mins = (int)seconds / 60;
    int secs = (int)seconds % 60;
    int ms   = (int)((seconds - std::floor(seconds)) * 100.0f);

    g.setColour(OP1Colors::textPrimary);
    juce::String timeStr = juce::String::formatted("%d:%02d.%02d", mins, secs, ms);
    g.drawText(timeStr, bounds.removeFromLeft(80), juce::Justification::centredLeft);

    // Recording indicator: rec dot in vuRed
    if (isRecording)
    {
        g.setColour(OP1Colors::vuRed);
        g.fillEllipse((float)bounds.getX() + 2.0f,
                       (float)bounds.getCentreY() - 3.5f, 7.0f, 7.0f);

        bounds.removeFromLeft(12);
        g.setFont(OP1LookAndFeel::getDisplayFont(9.0f));
        g.drawText("REC", bounds.removeFromLeft(28), juce::Justification::centredLeft);
    }
    else if (isPlaying)
    {
        // Play triangle indicator
        g.setColour(OP1Colors::vuGreen);
        juce::Path playIcon;
        float py = (float)bounds.getCentreY();
        float px = (float)bounds.getX() + 2.0f;
        playIcon.addTriangle(px, py - 4.0f, px, py + 4.0f, px + 7.0f, py);
        g.fillPath(playIcon);
    }
}
