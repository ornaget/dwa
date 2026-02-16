#include "OP1Display.h"
#include "../LookAndFeel/OP1LookAndFeel.h"

OP1Display::OP1Display()
{
    setOpaque(true);
}

void OP1Display::setEncoderLabels(const juce::String& e1, const juce::String& e2,
                                   const juce::String& e3, const juce::String& e4)
{
    encLabel[0] = e1; encLabel[1] = e2; encLabel[2] = e3; encLabel[3] = e4;
    repaint();
}

void OP1Display::setEncoderValues(float v1, float v2, float v3, float v4)
{
    encValue[0] = v1; encValue[1] = v2; encValue[2] = v3; encValue[3] = v4;
    repaint();
}

void OP1Display::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();

    // AMOLED-style black background with rounded corners (like OP-1 Field screen)
    g.setColour(OP1Colors::displayBg);
    g.fillRoundedRectangle(bounds.toFloat(), 12.0f);

    // Display border/bezel
    g.setColour(OP1Colors::displayFrame);
    g.drawRoundedRectangle(bounds.toFloat().reduced(1.0f), 12.0f, 2.0f);

    // Inner padding
    auto inner = bounds.reduced(8, 4);

    // Header bar (mode name, engine, tempo, battery)
    auto headerArea = inner.removeFromTop(22);
    drawHeader(g, headerArea);

    // Separator line
    g.setColour(OP1Colors::textDim);
    g.fillRect(inner.getX(), inner.getY(), inner.getWidth(), 1);
    inner.removeFromTop(3);

    // Main content area (delegated to screens)
    auto contentArea = inner.removeFromBottom(inner.getHeight() - 26);
    auto encoderArea = inner;

    // Encoder parameter bar
    drawEncoderBar(g, encoderArea);

    // Content from current screen
    if (onPaint)
        onPaint(g, contentArea);
}

void OP1Display::drawHeader(juce::Graphics& g, juce::Rectangle<int> area)
{
    auto font = OP1LookAndFeel::getOP1Font(14.0f);
    g.setFont(font);

    // Mode name (left)
    g.setColour(OP1Colors::synthColor);
    g.drawText(modeName, area.removeFromLeft(70), juce::Justification::centredLeft);

    // Engine name
    g.setColour(OP1Colors::textPrimary);
    g.drawText(engineName, area.removeFromLeft(80), juce::Justification::centredLeft);

    // Tempo (right side)
    auto tempoArea = area.removeFromRight(60);
    g.setColour(OP1Colors::textSecondary);
    g.drawText(juce::String(tempo, 1) + " BPM", tempoArea, juce::Justification::centredRight);

    // Battery
    auto batteryArea = area.removeFromRight(30);
    drawBattery(g, batteryArea);
}

void OP1Display::drawEncoderBar(juce::Graphics& g, juce::Rectangle<int> area)
{
    int encWidth = area.getWidth() / 4;

    for (int i = 0; i < 4; ++i)
    {
        auto encArea = area.removeFromLeft(encWidth);
        auto labelArea = encArea.removeFromTop(14);
        auto valueArea = encArea;

        // Label
        g.setFont(OP1LookAndFeel::getOP1Font(11.0f));
        g.setColour(encColors[i]);
        g.drawText(encLabel[i], labelArea, juce::Justification::centred);

        // Value bar
        auto barBounds = valueArea.reduced(4, 2).toFloat();
        g.setColour(OP1Colors::textDim);
        g.fillRoundedRectangle(barBounds, 2.0f);

        g.setColour(encColors[i]);
        auto filledBar = barBounds;
        filledBar.setWidth(barBounds.getWidth() * encValue[i]);
        g.fillRoundedRectangle(filledBar, 2.0f);
    }
}

void OP1Display::drawBattery(juce::Graphics& g, juce::Rectangle<int> area)
{
    auto battArea = area.reduced(2, 5);
    g.setColour(OP1Colors::textSecondary);
    g.drawRect(battArea, 1);

    // Battery nub
    g.fillRect(battArea.getRight(), battArea.getY() + 3, 2, battArea.getHeight() - 6);

    // Fill
    auto fillArea = battArea.reduced(2, 2);
    auto fillColour = batteryLevel > 0.2f ? OP1Colors::vuGreen : OP1Colors::vuRed;
    g.setColour(fillColour);
    fillArea.setWidth((int)(fillArea.getWidth() * batteryLevel));
    g.fillRect(fillArea);
}

void OP1Display::resized()
{
}
