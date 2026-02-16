#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../../Utils/OP1Colors.h"

// OP-1 Field styled button (transport, mode buttons, etc.)
class OP1Button : public juce::TextButton
{
public:
    enum class Style { Normal, Mode, Transport, Small };

    OP1Button(const juce::String& text, Style style = Style::Normal);
    ~OP1Button() override = default;

    void paintButton(juce::Graphics& g, bool isHighlighted, bool isDown) override;
    void setActive(bool active) { isActive = active; repaint(); }
    void setActiveColor(juce::Colour color) { activeColor = color; }

private:
    Style buttonStyle;
    bool isActive = false;
    juce::Colour activeColor = OP1Colors::encoderBlue;
};
