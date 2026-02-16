#include "VUMeter.h"

VUMeter::VUMeter()
{
    startTimerHz(30);
}

void VUMeter::setLevel(float left, float right)
{
    levelL = left;
    levelR = right;

    if (left > peakL) { peakL = left; peakHoldCounterL = 30; }
    if (right > peakR) { peakR = right; peakHoldCounterR = 30; }
}

void VUMeter::timerCallback()
{
    // Decay
    levelL *= 0.85f;
    levelR *= 0.85f;

    peakHoldCounterL--;
    peakHoldCounterR--;
    if (peakHoldCounterL <= 0) peakL *= 0.95f;
    if (peakHoldCounterR <= 0) peakR *= 0.95f;

    repaint();
}

void VUMeter::drawChannel(juce::Graphics& g, juce::Rectangle<float> bounds, float level, float peak)
{
    int numSegments = 12;
    float segmentHeight = bounds.getHeight() / (float)numSegments;
    float gap = 1.0f;

    for (int i = 0; i < numSegments; ++i)
    {
        float segLevel = (float)(numSegments - i) / (float)numSegments;
        auto segBounds = juce::Rectangle<float>(
            bounds.getX(),
            bounds.getY() + (float)i * segmentHeight + gap,
            bounds.getWidth(),
            segmentHeight - gap * 2.0f);

        juce::Colour segColor;
        if (segLevel > 0.85f)
            segColor = OP1Colors::vuRed;
        else if (segLevel > 0.6f)
            segColor = OP1Colors::vuYellow;
        else
            segColor = OP1Colors::vuGreen;

        if (level >= segLevel)
        {
            g.setColour(segColor);
            g.fillRoundedRectangle(segBounds, 1.0f);
        }
        else
        {
            g.setColour(segColor.withAlpha(0.15f));
            g.fillRoundedRectangle(segBounds, 1.0f);
        }

        // Peak indicator
        if (std::abs(peak - segLevel) < 1.0f / (float)numSegments)
        {
            g.setColour(segColor);
            g.fillRoundedRectangle(segBounds, 1.0f);
        }
    }
}

void VUMeter::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    float channelWidth = (bounds.getWidth() - 2.0f) / 2.0f;

    auto leftBounds = juce::Rectangle<float>(bounds.getX(), bounds.getY(),
                                              channelWidth, bounds.getHeight());
    auto rightBounds = juce::Rectangle<float>(bounds.getX() + channelWidth + 2.0f, bounds.getY(),
                                               channelWidth, bounds.getHeight());

    drawChannel(g, leftBounds, levelL, peakL);
    drawChannel(g, rightBounds, levelR, peakR);
}
