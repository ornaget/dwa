#include "SynthScreen.h"
#include "../LookAndFeel/OP1LookAndFeel.h"

void SynthScreen::paint(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    auto waveArea = bounds.removeFromTop(bounds.getHeight() / 2);
    auto bottomArea = bounds;

    auto adsrArea = bottomArea.removeFromLeft(bottomArea.getWidth() / 2);
    auto noteArea = bottomArea;

    drawWaveform(g, waveArea);
    drawADSR(g, adsrArea);
    drawNoteInfo(g, noteArea);
}

void SynthScreen::drawWaveform(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    if (waveformData.empty())
    {
        // Draw a default sine-ish wave
        juce::Path wavePath;
        float centerY = bounds.getCentreY();
        float amplitude = (float)bounds.getHeight() * 0.35f;

        wavePath.startNewSubPath((float)bounds.getX(), centerY);
        for (int x = 0; x < bounds.getWidth(); ++x)
        {
            float t = (float)x / (float)bounds.getWidth();
            float y = centerY - std::sin(t * juce::MathConstants<float>::twoPi * 3.0f) * amplitude;
            wavePath.lineTo((float)(bounds.getX() + x), y);
        }

        g.setColour(OP1Colors::synthColor);
        g.strokePath(wavePath, juce::PathStrokeType(2.0f));
    }
    else
    {
        juce::Path wavePath;
        float centerY = bounds.getCentreY();
        float amplitude = (float)bounds.getHeight() * 0.35f;
        int dataSize = (int)waveformData.size();

        wavePath.startNewSubPath((float)bounds.getX(), centerY);
        for (int x = 0; x < bounds.getWidth(); ++x)
        {
            int dataIndex = (int)((float)x / (float)bounds.getWidth() * (float)dataSize);
            dataIndex = juce::jlimit(0, dataSize - 1, dataIndex);
            float y = centerY - waveformData[(size_t)dataIndex] * amplitude;
            wavePath.lineTo((float)(bounds.getX() + x), y);
        }

        g.setColour(OP1Colors::synthColor);
        g.strokePath(wavePath, juce::PathStrokeType(2.0f));

        // Glow effect
        g.setColour(OP1Colors::synthColor.withAlpha(0.15f));
        g.strokePath(wavePath, juce::PathStrokeType(6.0f));
    }

    // Engine label
    g.setColour(OP1Colors::textSecondary);
    g.setFont(OP1LookAndFeel::getOP1Font(12.0f));
    g.drawText(engineName, bounds.removeFromBottom(16), juce::Justification::centred);
}

void SynthScreen::drawADSR(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    bounds.reduce(4, 4);
    g.setColour(OP1Colors::textDim);
    g.setFont(OP1LookAndFeel::getOP1Font(10.0f));
    g.drawText("ADSR", bounds.removeFromTop(12), juce::Justification::centredLeft);

    auto envBounds = bounds.toFloat();
    float w = envBounds.getWidth();
    float h = envBounds.getHeight();
    float x = envBounds.getX();
    float y = envBounds.getY();

    // Normalize ADSR to visual widths
    float totalTime = attack + decay + 0.2f + release;
    float aW = (attack / totalTime) * w;
    float dW = (decay / totalTime) * w;
    float sW = 0.2f / totalTime * w;
    float rW = (release / totalTime) * w;

    juce::Path adsrPath;
    adsrPath.startNewSubPath(x, y + h);
    adsrPath.lineTo(x + aW, y);                           // Attack
    adsrPath.lineTo(x + aW + dW, y + h * (1.0f - sustain)); // Decay to sustain
    adsrPath.lineTo(x + aW + dW + sW, y + h * (1.0f - sustain)); // Sustain hold
    adsrPath.lineTo(x + aW + dW + sW + rW, y + h);       // Release

    g.setColour(OP1Colors::waveformGreen);
    g.strokePath(adsrPath, juce::PathStrokeType(1.5f));

    // Fill under curve
    juce::Path fillPath = adsrPath;
    fillPath.lineTo(x, y + h);
    fillPath.closeSubPath();
    g.setColour(OP1Colors::waveformGreen.withAlpha(0.1f));
    g.fillPath(fillPath);
}

void SynthScreen::drawNoteInfo(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    bounds.reduce(4, 4);

    if (currentNote >= 0)
    {
        static const char* noteNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
        int octave = currentNote / 12 - 1;
        int noteIndex = currentNote % 12;

        g.setColour(OP1Colors::textPrimary);
        g.setFont(OP1LookAndFeel::getOP1BoldFont(24.0f));
        g.drawText(juce::String(noteNames[noteIndex]) + juce::String(octave),
                   bounds, juce::Justification::centred);
    }
    else
    {
        g.setColour(OP1Colors::textDim);
        g.setFont(OP1LookAndFeel::getOP1Font(12.0f));
        g.drawText("--", bounds, juce::Justification::centred);
    }
}
