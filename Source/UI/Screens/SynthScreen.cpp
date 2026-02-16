#include "SynthScreen.h"
#include "../LookAndFeel/OP1LookAndFeel.h"

void SynthScreen::paint(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    // Top 60% for waveform, bottom 40% split between ADSR and note display
    auto waveArea = bounds.removeFromTop((int)(bounds.getHeight() * 0.6f));
    auto bottomArea = bounds;

    auto adsrArea = bottomArea.removeFromLeft(bottomArea.getWidth() / 2);
    auto noteArea = bottomArea;

    drawWaveform(g, waveArea);
    drawADSR(g, adsrArea);
    drawNoteInfo(g, noteArea);
}

void SynthScreen::drawWaveform(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    juce::Path wavePath;
    float centerY = (float)bounds.getCentreY();
    float amplitude = (float)bounds.getHeight() * 0.35f;

    if (waveformData.empty())
    {
        // Default sine wave visualization
        wavePath.startNewSubPath((float)bounds.getX(), centerY);
        for (int x = 0; x < bounds.getWidth(); ++x)
        {
            float t = (float)x / (float)bounds.getWidth();
            float y = centerY - std::sin(t * juce::MathConstants<float>::twoPi * 3.0f) * amplitude;
            wavePath.lineTo((float)(bounds.getX() + x), y);
        }
    }
    else
    {
        int dataSize = (int)waveformData.size();

        wavePath.startNewSubPath((float)bounds.getX(),
            centerY - waveformData[0] * amplitude);

        for (int x = 1; x < bounds.getWidth(); ++x)
        {
            int dataIndex = (int)((float)x / (float)bounds.getWidth() * (float)dataSize);
            dataIndex = juce::jlimit(0, dataSize - 1, dataIndex);
            float y = centerY - waveformData[(size_t)dataIndex] * amplitude;
            wavePath.lineTo((float)(bounds.getX() + x), y);
        }
    }

    // Subtle glow layer: same path at 8% alpha, 5px stroke
    g.setColour(OP1Colors::synthColor.withAlpha(0.08f));
    g.strokePath(wavePath, juce::PathStrokeType(5.0f,
        juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Main waveform: synthColor, 1.5px anti-aliased stroke
    g.setColour(OP1Colors::synthColor);
    g.strokePath(wavePath, juce::PathStrokeType(1.5f,
        juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Engine name label at bottom of waveform area
    if (engineName.isNotEmpty())
    {
        g.setColour(OP1Colors::textSecondary);
        g.setFont(OP1LookAndFeel::getDisplayFont(10.0f));
        g.drawText(engineName.toUpperCase(),
                   bounds.removeFromBottom(14).reduced(4, 0),
                   juce::Justification::centredLeft);
    }
}

void SynthScreen::drawADSR(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    bounds.reduce(6, 4);

    // Label
    g.setColour(OP1Colors::textDim);
    g.setFont(OP1LookAndFeel::getDisplayFont(9.0f));
    g.drawText("ADSR", bounds.removeFromTop(12), juce::Justification::centredLeft);

    auto envBounds = bounds.reduced(0, 2).toFloat();
    float w = envBounds.getWidth();
    float h = envBounds.getHeight();
    float x = envBounds.getX();
    float y = envBounds.getY();

    // Normalize ADSR segments to visual widths
    float totalTime = attack + decay + 0.2f + release;
    float aW = (attack / totalTime) * w;
    float dW = (decay / totalTime) * w;
    float sW = (0.2f / totalTime) * w;
    float rW = (release / totalTime) * w;

    juce::Path adsrPath;
    adsrPath.startNewSubPath(x, y + h);
    adsrPath.lineTo(x + aW, y);                                       // Attack peak
    adsrPath.lineTo(x + aW + dW, y + h * (1.0f - sustain));          // Decay to sustain
    adsrPath.lineTo(x + aW + dW + sW, y + h * (1.0f - sustain));    // Sustain hold
    adsrPath.lineTo(x + aW + dW + sW + rW, y + h);                  // Release

    // Fill underneath at 6% alpha
    juce::Path fillPath = adsrPath;
    fillPath.lineTo(x, y + h);
    fillPath.closeSubPath();
    g.setColour(OP1Colors::waveGreen.withAlpha(0.06f));
    g.fillPath(fillPath);

    // Thin envelope line in waveGreen, 1px stroke
    g.setColour(OP1Colors::waveGreen);
    g.strokePath(adsrPath, juce::PathStrokeType(1.0f,
        juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

void SynthScreen::drawNoteInfo(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    bounds.reduce(6, 4);

    if (currentNote >= 0)
    {
        static const char* noteNames[] = {
            "C", "C#", "D", "D#", "E", "F",
            "F#", "G", "G#", "A", "A#", "B"
        };
        int octave = currentNote / 12 - 1;
        int noteIndex = currentNote % 12;
        juce::String noteStr = juce::String(noteNames[noteIndex]) + juce::String(octave);

        // Large bold note name in textBright
        g.setColour(OP1Colors::textBright);
        g.setFont(OP1LookAndFeel::getDisplayBoldFont(28.0f));
        g.drawText(noteStr, bounds, juce::Justification::centred);
    }
    else
    {
        // Inactive: "--" in textDim
        g.setColour(OP1Colors::textDim);
        g.setFont(OP1LookAndFeel::getDisplayFont(20.0f));
        g.drawText("--", bounds, juce::Justification::centred);
    }
}
