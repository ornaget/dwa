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
    // Smoother decay: 0.9f multiplier (was 0.85f)
    levelL *= 0.9f;
    levelR *= 0.9f;

    peakHoldCounterL--;
    peakHoldCounterR--;
    if (peakHoldCounterL <= 0) peakL *= 0.95f;
    if (peakHoldCounterR <= 0) peakR *= 0.95f;

    repaint();
}

void VUMeter::drawChannel(juce::Graphics& g, juce::Rectangle<float> bounds, float level, float peak)
{
    int numSegments = 16;
    float gap = 1.5f;
    float segmentHeight = (bounds.getHeight() - gap * (float)(numSegments - 1)) / (float)numSegments;

    for (int i = 0; i < numSegments; ++i)
    {
        // Segment level: top segment = 1.0, bottom segment = lowest
        float segLevel = (float)(numSegments - i) / (float)numSegments;

        float yPos = bounds.getY() + (float)i * (segmentHeight + gap);
        auto segBounds = juce::Rectangle<float>(
            bounds.getX(),
            yPos,
            bounds.getWidth(),
            segmentHeight);

        // Color: green below 70%, yellow 70-85%, red above 85%
        juce::Colour segColor;
        if (segLevel > 0.85f)
            segColor = OP1Colors::vuRed;
        else if (segLevel > 0.70f)
            segColor = OP1Colors::vuYellow;
        else
            segColor = OP1Colors::vuGreen;

        // Peak hold: draw segment at peak level with full brightness
        bool isPeakSegment = (peak >= segLevel) && (peak < segLevel + 1.0f / (float)numSegments);

        if (level >= segLevel || isPeakSegment)
        {
            // Active segments: full color
            g.setColour(segColor);
            g.fillRect(segBounds);
        }
        else
        {
            // Inactive: color at 8% alpha (very subtle)
            g.setColour(segColor.withAlpha(0.08f));
            g.fillRect(segBounds);
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
