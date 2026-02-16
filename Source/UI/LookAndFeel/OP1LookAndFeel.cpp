#include "OP1LookAndFeel.h"

OP1LookAndFeel::OP1LookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId, OP1Colors::body);
    setColour(juce::Label::textColourId, OP1Colors::textPrimary);
    setColour(juce::TextButton::buttonColourId, OP1Colors::buttonFace);
    setColour(juce::TextButton::textColourOffId, OP1Colors::buttonText);
}

// ─── Font Helpers ────────────────────────────────────────────────────────────

juce::Font OP1LookAndFeel::getDisplayFont(float height)
{
    return juce::Font(juce::FontOptions("Helvetica Neue", height, juce::Font::plain));
}

juce::Font OP1LookAndFeel::getDisplayBoldFont(float height)
{
    return juce::Font(juce::FontOptions("Helvetica Neue", height, juce::Font::bold));
}

juce::Font OP1LookAndFeel::getBodyFont(float height)
{
    return juce::Font(juce::FontOptions("Helvetica Neue", height, juce::Font::plain));
}

// ─── Premium Rotary Encoder ─────────────────────────────────────────────────

void OP1LookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                       float sliderPos, float /*rotaryStartAngle*/,
                                       float /*rotaryEndAngle*/, juce::Slider& slider)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto centre = bounds.getCentre();
    auto encoderColour = slider.findColour(juce::Slider::thumbColourId);

    // --- Outer shadow (subtle drop shadow) ---
    {
        auto shadowAlpha = 0.15f;
        for (int i = 3; i >= 1; --i)
        {
            g.setColour(juce::Colours::black.withAlpha(shadowAlpha * (1.0f - (float)i / 4.0f)));
            g.fillEllipse(centre.x - radius - 1.0f, centre.y - radius + (float)i * 1.0f,
                          radius * 2.0f + 2.0f, radius * 2.0f + 2.0f);
        }
    }

    // --- Gradient body ---
    {
        juce::ColourGradient bodyGrad(encoderColour.brighter(0.15f),
                                       centre.x, centre.y - radius,
                                       encoderColour.darker(0.1f),
                                       centre.x, centre.y + radius,
                                       false);
        g.setGradientFill(bodyGrad);
        g.fillEllipse(centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f);
    }

    // --- Subtle rim / edge ---
    {
        g.setColour(encoderColour.darker(0.25f).withAlpha(0.6f));
        g.drawEllipse(centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f, 0.75f);
    }

    // --- Concentric ring detail (inner groove) ---
    {
        auto innerRingRadius = radius * 0.72f;
        g.setColour(encoderColour.darker(0.12f).withAlpha(0.5f));
        g.drawEllipse(centre.x - innerRingRadius, centre.y - innerRingRadius,
                      innerRingRadius * 2.0f, innerRingRadius * 2.0f, 0.5f);

        auto innerRingRadius2 = radius * 0.55f;
        g.setColour(encoderColour.darker(0.08f).withAlpha(0.3f));
        g.drawEllipse(centre.x - innerRingRadius2, centre.y - innerRingRadius2,
                      innerRingRadius2 * 2.0f, innerRingRadius2 * 2.0f, 0.4f);
    }

    // --- Soft highlight (top-left specular) ---
    {
        auto highlightRadius = radius * 0.35f;
        auto hx = centre.x - radius * 0.22f;
        auto hy = centre.y - radius * 0.32f;

        juce::ColourGradient highlightGrad(juce::Colours::white.withAlpha(0.25f),
                                            hx, hy,
                                            juce::Colours::white.withAlpha(0.0f),
                                            hx + highlightRadius, hy + highlightRadius,
                                            true);
        g.setGradientFill(highlightGrad);
        g.fillEllipse(hx - highlightRadius, hy - highlightRadius,
                      highlightRadius * 2.0f, highlightRadius * 2.0f);
    }

    // --- Clean dot indicator ---
    {
        float angle = -juce::MathConstants<float>::pi * 0.75f
                      + sliderPos * juce::MathConstants<float>::pi * 1.5f;
        float dotDistance = radius * 0.62f;
        float dotRadius = radius * 0.08f;
        juce::Point<float> dotPos(centre.x + std::sin(angle) * dotDistance,
                                   centre.y - std::cos(angle) * dotDistance);

        g.setColour(juce::Colours::white.withAlpha(0.95f));
        g.fillEllipse(dotPos.x - dotRadius, dotPos.y - dotRadius,
                      dotRadius * 2.0f, dotRadius * 2.0f);
    }
}

// ─── Clean Button Rendering ──────────────────────────────────────────────────

void OP1LookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                            const juce::Colour& backgroundColour,
                                            bool isHighlighted, bool isDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(1.0f);
    auto cornerRadius = 5.0f;

    // Determine the face and edge colors based on state
    auto faceColour = backgroundColour;
    auto edgeColour = OP1Colors::buttonEdge;

    if (isDown)
    {
        faceColour = OP1Colors::buttonActive;
        edgeColour = OP1Colors::buttonActive.darker(0.15f);
    }
    else if (isHighlighted)
    {
        faceColour = faceColour.brighter(0.06f);
    }

    // Subtle bottom shadow
    g.setColour(OP1Colors::bodyShadow.withAlpha(0.35f));
    g.fillRoundedRectangle(bounds.translated(0.0f, 1.5f), cornerRadius);

    // Button body with slight gradient
    {
        juce::ColourGradient bodyGrad(faceColour.brighter(0.05f),
                                       bounds.getX(), bounds.getY(),
                                       faceColour.darker(0.03f),
                                       bounds.getX(), bounds.getBottom(),
                                       false);
        g.setGradientFill(bodyGrad);
        g.fillRoundedRectangle(bounds, cornerRadius);
    }

    // Clean edge stroke
    g.setColour(edgeColour);
    g.drawRoundedRectangle(bounds, cornerRadius, 0.75f);
}

// ─── Label Drawing ───────────────────────────────────────────────────────────

void OP1LookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    g.setColour(label.findColour(juce::Label::textColourId));
    g.setFont(getDisplayFont(label.getFont().getHeight()));
    g.drawFittedText(label.getText(), label.getLocalBounds(),
                     label.getJustificationType(), 1);
}

juce::Font OP1LookAndFeel::getTextButtonFont(juce::TextButton&, int buttonHeight)
{
    return getBodyFont((float)buttonHeight * 0.45f);
}

juce::Font OP1LookAndFeel::getLabelFont(juce::Label& label)
{
    return getDisplayFont(label.getFont().getHeight());
}
