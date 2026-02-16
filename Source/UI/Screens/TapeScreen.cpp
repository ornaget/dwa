#include "TapeScreen.h"
#include "../LookAndFeel/OP1LookAndFeel.h"

void TapeScreen::setTrackData(int track, const std::vector<float>& data)
{
    if (track >= 0 && track < 4)
        trackWaveforms[track] = data;
}

void TapeScreen::paint(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    auto reelArea = bounds.removeFromLeft(bounds.getWidth() / 3);
    auto trackArea = bounds.removeFromTop(bounds.getHeight() - 20);
    auto infoArea = bounds;

    drawTapeReels(g, reelArea);
    drawTracks(g, trackArea);
    drawPlayhead(g, trackArea);
    drawTransportInfo(g, infoArea);
}

void TapeScreen::drawTapeReels(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    float reelSize = juce::jmin((float)bounds.getWidth() * 0.35f, (float)bounds.getHeight() * 0.4f);

    // Left reel (supply)
    float lx = (float)bounds.getX() + (float)bounds.getWidth() * 0.3f;
    float ly = (float)bounds.getCentreY() - reelSize * 0.3f;

    // Right reel (take-up)
    float rx = (float)bounds.getX() + (float)bounds.getWidth() * 0.7f;
    float ry = ly;

    // Reel sizes based on position (more tape on right = further along)
    float leftReelSize = reelSize * (0.5f + (1.0f - playPosition) * 0.5f);
    float rightReelSize = reelSize * (0.5f + playPosition * 0.5f);

    if (isPlaying) tapeReelAngle += 0.05f;

    for (int reel = 0; reel < 2; ++reel)
    {
        float cx = reel == 0 ? lx : rx;
        float cy = reel == 0 ? ly : ry;
        float size = reel == 0 ? leftReelSize : rightReelSize;

        // Reel circle
        g.setColour(OP1Colors::textDim);
        g.drawEllipse(cx - size, cy - size, size * 2, size * 2, 1.5f);

        // Hub
        g.setColour(OP1Colors::displayFrame);
        g.fillEllipse(cx - 4, cy - 4, 8, 8);

        // Spokes
        float direction = reel == 0 ? -1.0f : 1.0f;
        for (int spoke = 0; spoke < 3; ++spoke)
        {
            float angle = tapeReelAngle * direction + (float)spoke * juce::MathConstants<float>::twoPi / 3.0f;
            float sx = cx + std::cos(angle) * size * 0.8f;
            float sy = cy + std::sin(angle) * size * 0.8f;
            g.setColour(OP1Colors::textDim.withAlpha(0.5f));
            g.drawLine(cx, cy, sx, sy, 0.5f);
        }
    }

    // Tape between reels
    g.setColour(OP1Colors::textSecondary.withAlpha(0.3f));
    g.drawLine(lx + leftReelSize, ly, rx - rightReelSize, ry, 1.0f);
}

void TapeScreen::drawTracks(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    juce::Colour trackColors[] = { OP1Colors::track1, OP1Colors::track2,
                                    OP1Colors::track3, OP1Colors::track4 };
    int trackHeight = bounds.getHeight() / 4;

    for (int t = 0; t < 4; ++t)
    {
        auto trackBounds = juce::Rectangle<int>(bounds.getX(), bounds.getY() + t * trackHeight,
                                                 bounds.getWidth(), trackHeight);
        bool isActive = (t == activeTrack);

        // Track background
        g.setColour(trackColors[t].withAlpha(isActive ? 0.1f : 0.03f));
        g.fillRect(trackBounds);

        // Track label
        g.setColour(isActive ? trackColors[t] : trackColors[t].withAlpha(0.3f));
        g.setFont(OP1LookAndFeel::getOP1Font(9.0f));
        g.drawText(juce::String(t + 1), trackBounds.removeFromLeft(14), juce::Justification::centred);

        // Track waveform
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
                float viewEnd = playPosition + 0.1f;
                float t_pos = viewStart + (float)x / (float)trackBounds.getWidth() * (viewEnd - viewStart);
                int idx = (int)(t_pos * (float)dataSize);
                idx = juce::jlimit(0, dataSize - 1, idx);
                float y = cy - trackWaveforms[t][(size_t)idx] * amp;
                wavePath.lineTo((float)(trackBounds.getX() + x), y);
            }

            g.setColour(trackColors[t].withAlpha(isActive ? 0.8f : 0.3f));
            g.strokePath(wavePath, juce::PathStrokeType(1.0f));
        }
        else
        {
            // Empty track indicator
            g.setColour(trackColors[t].withAlpha(0.08f));
            g.drawLine((float)trackBounds.getX(), (float)trackBounds.getCentreY(),
                       (float)trackBounds.getRight(), (float)trackBounds.getCentreY(), 0.5f);
        }

        // Track separator
        g.setColour(OP1Colors::textDim.withAlpha(0.2f));
        g.drawHorizontalLine(trackBounds.getBottom(), (float)trackBounds.getX(), (float)trackBounds.getRight());
    }

    // Loop region overlay
    if (loopStart > 0.0f || loopEnd < 1.0f)
    {
        float lsx = bounds.getX() + loopStart * (float)bounds.getWidth();
        float lex = bounds.getX() + loopEnd * (float)bounds.getWidth();

        g.setColour(OP1Colors::waveformYellow.withAlpha(0.1f));
        g.fillRect(lsx, (float)bounds.getY(), lex - lsx, (float)bounds.getHeight());

        g.setColour(OP1Colors::waveformYellow.withAlpha(0.5f));
        g.drawLine(lsx, (float)bounds.getY(), lsx, (float)bounds.getBottom(), 1.0f);
        g.drawLine(lex, (float)bounds.getY(), lex, (float)bounds.getBottom(), 1.0f);
    }
}

void TapeScreen::drawPlayhead(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Playhead at center
    float x = (float)bounds.getCentreX();
    g.setColour(isRecording ? OP1Colors::vuRed : OP1Colors::textPrimary);
    g.drawLine(x, (float)bounds.getY(), x, (float)bounds.getBottom(), 1.5f);

    // Playhead triangle
    juce::Path triangle;
    triangle.addTriangle(x - 4, (float)bounds.getY(), x + 4, (float)bounds.getY(), x, (float)bounds.getY() + 6);
    g.fillPath(triangle);
}

void TapeScreen::drawTransportInfo(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    g.setFont(OP1LookAndFeel::getOP1Font(10.0f));

    // Position
    float seconds = playPosition * 360.0f; // 6 min tape
    int mins = (int)seconds / 60;
    int secs = (int)seconds % 60;
    int ms = (int)((seconds - std::floor(seconds)) * 100.0f);

    g.setColour(OP1Colors::textPrimary);
    juce::String timeStr = juce::String::formatted("%d:%02d.%02d", mins, secs, ms);
    g.drawText(timeStr, bounds.removeFromLeft(80), juce::Justification::centredLeft);

    // Recording indicator
    if (isRecording)
    {
        g.setColour(OP1Colors::vuRed);
        g.fillEllipse((float)bounds.getX(), (float)bounds.getCentreY() - 4, 8, 8);
        g.drawText("REC", bounds.removeFromLeft(30), juce::Justification::centredLeft);
    }
    else if (isPlaying)
    {
        g.setColour(OP1Colors::vuGreen);
        juce::Path playIcon;
        float py = (float)bounds.getCentreY();
        playIcon.addTriangle((float)bounds.getX(), py - 5, (float)bounds.getX(), py + 5,
                             (float)bounds.getX() + 8, py);
        g.fillPath(playIcon);
    }
}
