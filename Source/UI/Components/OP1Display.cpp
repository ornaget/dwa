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
    auto bounds = getLocalBounds().toFloat();

    // True black background with very subtle rounded corners
    g.setColour(OP1Colors::displayBg);
    g.fillRoundedRectangle(bounds, 8.0f);

    // Very thin border in displayBorder color
    g.setColour(OP1Colors::displayBorder);
    g.drawRoundedRectangle(bounds.reduced(0.25f), 8.0f, 0.5f);

    // Inner padding - generous horizontal, tighter vertical for clean spacing
    auto inner = bounds.reduced(14.0f, 8.0f).toNearestInt();

    // ─── Header bar ──────────────────────────────────────────────────
    auto headerArea = inner.removeFromTop(18);
    drawHeader(g, headerArea);

    // Spacing above separator
    inner.removeFromTop(5);

    // Thin 1px separator line in textMuted
    g.setColour(OP1Colors::textMuted);
    g.fillRect(inner.getX(), inner.getY(), inner.getWidth(), 1);
    inner.removeFromTop(1);

    // Spacing below separator
    inner.removeFromTop(6);

    // ─── Encoder parameter bar ───────────────────────────────────────
    auto encoderArea = inner.removeFromTop(30);
    drawEncoderBar(g, encoderArea);

    // Spacing before content
    inner.removeFromTop(4);

    // ─── Content from current screen ─────────────────────────────────
    if (onPaint)
        onPaint(g, inner);
}

void OP1Display::drawHeader(juce::Graphics& g, juce::Rectangle<int> area)
{
    auto boldFont = OP1LookAndFeel::getDisplayBoldFont(12.0f);
    auto regularFont = OP1LookAndFeel::getDisplayFont(12.0f);

    // Mode name in accent color (left-aligned)
    g.setFont(boldFont);
    g.setColour(OP1Colors::synthColor);
    g.drawText(modeName, area.removeFromLeft(56), juce::Justification::centredLeft);

    // Small gap
    area.removeFromLeft(6);

    // Engine name in textPrimary
    g.setFont(regularFont);
    g.setColour(OP1Colors::textPrimary);
    g.drawText(engineName, area.removeFromLeft(72), juce::Justification::centredLeft);

    // Battery (right side, before tempo)
    auto batteryArea = area.removeFromRight(26);
    drawBattery(g, batteryArea);

    // Small gap between battery and tempo
    area.removeFromRight(6);

    // Tempo right-aligned in textSecondary
    auto tempoArea = area.removeFromRight(64);
    g.setFont(regularFont);
    g.setColour(OP1Colors::textSecondary);
    g.drawText(juce::String(tempo, 1) + " BPM", tempoArea, juce::Justification::centredRight);
}

void OP1Display::drawEncoderBar(juce::Graphics& g, juce::Rectangle<int> area)
{
    int encWidth = area.getWidth() / 4;
    auto labelFont = OP1LookAndFeel::getDisplayBoldFont(10.0f);

    for (int i = 0; i < 4; ++i)
    {
        auto encArea = area.removeFromLeft(encWidth);

        // Label in encoder color
        auto labelArea = encArea.removeFromTop(13);
        g.setFont(labelFont);
        g.setColour(encColors[i]);
        g.drawText(encLabel[i], labelArea, juce::Justification::centred);

        // Small gap between label and bar
        encArea.removeFromTop(3);

        // Horizontal value bar
        auto barBounds = encArea.removeFromTop(5).reduced(6, 0).toFloat();

        // Bar track in textMuted
        g.setColour(OP1Colors::textMuted);
        g.fillRoundedRectangle(barBounds, 1.5f);

        // Filled portion in encoder color
        if (encValue[i] > 0.0f)
        {
            auto filledBar = barBounds;
            filledBar.setWidth(juce::jmax(3.0f, barBounds.getWidth() * encValue[i]));
            g.setColour(encColors[i]);
            g.fillRoundedRectangle(filledBar, 1.5f);
        }
    }
}

void OP1Display::drawBattery(juce::Graphics& g, juce::Rectangle<int> area)
{
    // Center the battery icon vertically
    auto battArea = area.reduced(1, 4).toFloat();
    auto battBody = battArea.removeFromLeft(battArea.getWidth() - 2.5f);

    // Rounded rectangle outline
    g.setColour(OP1Colors::textSecondary);
    g.drawRoundedRectangle(battBody, 1.5f, 0.75f);

    // Nub (positive terminal) as a small rounded rect
    auto nubArea = juce::Rectangle<float>(battBody.getRight() + 0.5f,
                                           battBody.getCentreY() - 2.0f,
                                           1.5f, 4.0f);
    g.fillRoundedRectangle(nubArea, 0.5f);

    // Fill level
    auto fillArea = battBody.reduced(1.5f, 1.5f);
    auto fillColour = batteryLevel > 0.2f ? OP1Colors::vuGreen : OP1Colors::vuRed;
    g.setColour(fillColour);
    auto filledWidth = fillArea.getWidth() * batteryLevel;
    g.fillRoundedRectangle(fillArea.getX(), fillArea.getY(),
                           juce::jmax(1.0f, filledWidth), fillArea.getHeight(), 0.75f);
}

void OP1Display::resized()
{
}
