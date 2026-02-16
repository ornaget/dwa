#include "OP1Button.h"
#include "../LookAndFeel/OP1LookAndFeel.h"

OP1Button::OP1Button(const juce::String& text, Style style)
    : juce::TextButton(text), buttonStyle(style)
{
}

void OP1Button::paintButton(juce::Graphics& g, bool isHighlighted, bool isDown)
{
    auto bounds = getLocalBounds().toFloat().reduced(1.0f);

    juce::Colour bgColor;
    juce::Colour textColor;
    float cornerRadius = 4.0f;

    switch (buttonStyle)
    {
        case Style::Mode:
        {
            cornerRadius = 5.0f;

            if (isActive)
            {
                bgColor = activeColor;
                textColor = OP1Colors::buttonTextLight;
            }
            else
            {
                bgColor = OP1Colors::buttonFace;
                textColor = OP1Colors::buttonText;
            }

            // When pressed: translate down and darken slightly
            if (isDown)
            {
                bounds = bounds.translated(0.0f, 0.5f);
                bgColor = bgColor.darker(0.08f);
            }
            else if (isHighlighted)
            {
                bgColor = bgColor.brighter(0.04f);
            }

            // Subtle 1px shadow underneath
            if (!isDown)
            {
                g.setColour(juce::Colours::black.withAlpha(0.15f));
                g.fillRoundedRectangle(bounds.translated(0.0f, 1.0f), cornerRadius);
            }

            // Button body
            g.setColour(bgColor);
            g.fillRoundedRectangle(bounds, cornerRadius);

            // Top half white 3% highlight for depth
            {
                auto highlightBounds = bounds;
                highlightBounds.setHeight(bounds.getHeight() * 0.5f);
                g.setColour(juce::Colours::white.withAlpha(0.03f));
                g.fillRoundedRectangle(highlightBounds, cornerRadius);
            }

            break;
        }

        case Style::Transport:
        {
            // Pill-shaped (full radius corners)
            cornerRadius = bounds.getHeight() / 2.0f;

            if (isActive)
            {
                // Active play = vuGreen, active rec = vuRed
                // Default active is vuGreen; callers set activeColor to vuRed for rec
                bgColor = activeColor;
            }
            else
            {
                bgColor = OP1Colors::keyBlack;
            }

            // Text always white for transport buttons
            textColor = juce::Colours::white;

            if (isDown)
            {
                bounds = bounds.translated(0.0f, 0.5f);
                bgColor = bgColor.darker(0.1f);
            }
            else if (isHighlighted)
            {
                bgColor = bgColor.brighter(0.06f);
            }

            // Shadow
            if (!isDown)
            {
                g.setColour(juce::Colours::black.withAlpha(0.15f));
                g.fillRoundedRectangle(bounds.translated(0.0f, 1.0f), cornerRadius);
            }

            // Button body
            g.setColour(bgColor);
            g.fillRoundedRectangle(bounds, cornerRadius);

            break;
        }

        case Style::Small:
        {
            cornerRadius = 4.0f;
            bgColor = OP1Colors::buttonFace;
            textColor = OP1Colors::buttonText;

            if (isDown)
            {
                bounds = bounds.translated(0.0f, 0.5f);
                bgColor = bgColor.darker(0.06f);
            }
            else if (isHighlighted)
            {
                bgColor = bgColor.brighter(0.04f);
            }

            // Shadow
            if (!isDown)
            {
                g.setColour(juce::Colours::black.withAlpha(0.12f));
                g.fillRoundedRectangle(bounds.translated(0.0f, 1.0f), cornerRadius);
            }

            // Button body
            g.setColour(bgColor);
            g.fillRoundedRectangle(bounds, cornerRadius);

            break;
        }

        default: // Style::Normal
        {
            cornerRadius = 5.0f;
            bgColor = OP1Colors::buttonFace;
            textColor = OP1Colors::buttonText;

            if (isDown)
            {
                bounds = bounds.translated(0.0f, 0.5f);
                bgColor = bgColor.darker(0.06f);
            }
            else if (isHighlighted)
            {
                bgColor = bgColor.brighter(0.04f);
            }

            // Shadow
            if (!isDown)
            {
                g.setColour(juce::Colours::black.withAlpha(0.12f));
                g.fillRoundedRectangle(bounds.translated(0.0f, 1.0f), cornerRadius);
            }

            // Button body
            g.setColour(bgColor);
            g.fillRoundedRectangle(bounds, cornerRadius);

            break;
        }
    }

    // Text - all buttons use OP1LookAndFeel::getBodyFont
    g.setColour(textColor);
    g.setFont(OP1LookAndFeel::getBodyFont(bounds.getHeight() * 0.4f));
    g.drawText(getButtonText(), bounds, juce::Justification::centred);
}
