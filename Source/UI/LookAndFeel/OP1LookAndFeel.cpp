#include "OP1LookAndFeel.h"

OP1LookAndFeel::OP1LookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId, OP1Colors::bodyColor);
    setColour(juce::Label::textColourId, OP1Colors::textPrimary);
    setColour(juce::TextButton::buttonColourId, OP1Colors::displayFrame);
    setColour(juce::TextButton::textColourOffId, OP1Colors::textPrimary);
}

juce::Font OP1LookAndFeel::getOP1Font(float height)
{
    return juce::Font(juce::FontOptions(height));
}

juce::Font OP1LookAndFeel::getOP1BoldFont(float height)
{
    return juce::Font(juce::FontOptions(height).withStyle("Bold"));
}

void OP1LookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                       float sliderPos, float /*rotaryStartAngle*/,
                                       float /*rotaryEndAngle*/, juce::Slider& slider)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto centre = bounds.getCentre();

    // Encoder base (circle)
    auto encoderColour = slider.findColour(juce::Slider::thumbColourId);

    // Shadow
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.fillEllipse(centre.x - radius + 2, centre.y - radius + 2, radius * 2, radius * 2);

    // Main encoder body
    g.setColour(encoderColour);
    g.fillEllipse(centre.x - radius, centre.y - radius, radius * 2, radius * 2);

    // Inner circle (knob indent)
    g.setColour(encoderColour.darker(0.2f));
    g.fillEllipse(centre.x - radius * 0.7f, centre.y - radius * 0.7f,
                  radius * 1.4f, radius * 1.4f);

    // Highlight
    g.setColour(encoderColour.brighter(0.3f));
    g.fillEllipse(centre.x - radius * 0.15f, centre.y - radius * 0.6f,
                  radius * 0.3f, radius * 0.3f);

    // Position indicator line
    float angle = -juce::MathConstants<float>::pi * 0.75f
                  + sliderPos * juce::MathConstants<float>::pi * 1.5f;
    float lineLength = radius * 0.7f;
    juce::Point<float> endPoint(centre.x + std::sin(angle) * lineLength,
                                 centre.y - std::cos(angle) * lineLength);

    g.setColour(juce::Colours::white);
    g.drawLine(centre.x, centre.y, endPoint.x, endPoint.y, 2.0f);
}

void OP1LookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                            const juce::Colour& backgroundColour,
                                            bool isHighlighted, bool isDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);
    auto cornerRadius = 4.0f;

    auto baseColour = backgroundColour;
    if (isDown)
        baseColour = baseColour.brighter(0.2f);
    else if (isHighlighted)
        baseColour = baseColour.brighter(0.1f);

    // Button shadow
    g.setColour(juce::Colours::black.withAlpha(0.2f));
    g.fillRoundedRectangle(bounds.translated(0, 2), cornerRadius);

    // Button body
    g.setColour(baseColour);
    g.fillRoundedRectangle(bounds, cornerRadius);

    // Subtle border
    g.setColour(baseColour.brighter(0.1f));
    g.drawRoundedRectangle(bounds, cornerRadius, 1.0f);
}

void OP1LookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    g.setColour(label.findColour(juce::Label::textColourId));
    g.setFont(getOP1Font(label.getFont().getHeight()));
    g.drawFittedText(label.getText(), label.getLocalBounds(),
                     label.getJustificationType(), 1);
}

juce::Font OP1LookAndFeel::getTextButtonFont(juce::TextButton&, int buttonHeight)
{
    return getOP1Font((float)buttonHeight * 0.5f);
}

juce::Font OP1LookAndFeel::getLabelFont(juce::Label& label)
{
    return getOP1Font(label.getFont().getHeight());
}
