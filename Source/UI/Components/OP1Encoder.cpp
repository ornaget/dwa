#include "OP1Encoder.h"
#include "../LookAndFeel/OP1LookAndFeel.h"

OP1Encoder::OP1Encoder(const juce::String& name, juce::Colour color)
    : encoderColor(color)
{
    setName(name);
    setSliderStyle(juce::Slider::RotaryVerticalDrag);
    setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    setRange(0.0, 1.0, 0.001);
    setValue(0.5);
    setColour(juce::Slider::thumbColourId, encoderColor);
    setRotaryParameters(juce::MathConstants<float>::pi * 1.25f,
                        juce::MathConstants<float>::pi * 2.75f, true);
}

void OP1Encoder::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();

    // Draw the encoder knob
    auto knobArea = bounds.reduced(4);
    auto knobBounds = knobArea.toFloat();
    auto radius = juce::jmin(knobBounds.getWidth(), knobBounds.getHeight()) / 2.0f;
    auto centre = knobBounds.getCentre();

    // Shadow
    g.setColour(juce::Colours::black.withAlpha(0.25f));
    g.fillEllipse(centre.x - radius + 2, centre.y - radius + 3, radius * 2, radius * 2);

    // Outer ring
    g.setColour(encoderColor.darker(0.3f));
    g.fillEllipse(centre.x - radius, centre.y - radius, radius * 2, radius * 2);

    // Main encoder surface
    float innerRadius = radius * 0.88f;
    g.setColour(encoderColor);
    g.fillEllipse(centre.x - innerRadius, centre.y - innerRadius,
                  innerRadius * 2, innerRadius * 2);

    // Top highlight (glossy look)
    juce::ColourGradient highlight(encoderColor.brighter(0.4f), centre.x, centre.y - innerRadius * 0.5f,
                                    encoderColor.withAlpha(0.0f), centre.x, centre.y + innerRadius * 0.3f, false);
    g.setGradientFill(highlight);
    g.fillEllipse(centre.x - innerRadius * 0.7f, centre.y - innerRadius * 0.8f,
                  innerRadius * 1.4f, innerRadius * 1.0f);

    // Position indicator dot
    float sliderPos = (float)valueToProportionOfLength(getValue());
    float angle = juce::MathConstants<float>::pi * 1.25f
                  + sliderPos * juce::MathConstants<float>::pi * 1.5f;
    float dotRadius = radius * 0.12f;
    float dotDist = innerRadius * 0.65f;
    float dotX = centre.x + std::sin(angle) * dotDist;
    float dotY = centre.y - std::cos(angle) * dotDist;

    g.setColour(juce::Colours::white);
    g.fillEllipse(dotX - dotRadius, dotY - dotRadius, dotRadius * 2, dotRadius * 2);
}
