#include "OP1Button.h"

OP1Button::OP1Button(const juce::String& text, Style style)
    : juce::TextButton(text), buttonStyle(style)
{
}

void OP1Button::paintButton(juce::Graphics& g, bool isHighlighted, bool isDown)
{
    auto bounds = getLocalBounds().toFloat().reduced(1.0f);

    juce::Colour bgColor;
    juce::Colour textColor = OP1Colors::textPrimary;
    float cornerRadius = 4.0f;

    switch (buttonStyle)
    {
        case Style::Mode:
            bgColor = isActive ? activeColor : OP1Colors::displayFrame;
            textColor = isActive ? juce::Colours::white : OP1Colors::textSecondary;
            cornerRadius = 6.0f;
            break;
        case Style::Transport:
            bgColor = isActive ? OP1Colors::vuGreen : OP1Colors::keyBlack;
            cornerRadius = bounds.getHeight() / 2.0f; // pill shape
            break;
        case Style::Small:
            bgColor = OP1Colors::displayFrame;
            cornerRadius = 3.0f;
            break;
        default:
            bgColor = OP1Colors::bodyDark;
            break;
    }

    if (isDown)
        bgColor = bgColor.brighter(0.3f);
    else if (isHighlighted)
        bgColor = bgColor.brighter(0.1f);

    // Shadow
    g.setColour(juce::Colours::black.withAlpha(0.2f));
    g.fillRoundedRectangle(bounds.translated(0, 1.5f), cornerRadius);

    // Button body
    g.setColour(bgColor);
    g.fillRoundedRectangle(bounds, cornerRadius);

    // Text
    g.setColour(textColor);
    g.setFont(juce::Font(juce::FontOptions(bounds.getHeight() * 0.4f)));
    g.drawText(getButtonText(), bounds, juce::Justification::centred);
}
