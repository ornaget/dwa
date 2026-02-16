#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../../Utils/OP1Colors.h"

// OP-1 Field style rotary encoder knob
class OP1Encoder : public juce::Slider
{
public:
    OP1Encoder(const juce::String& name, juce::Colour color);
    ~OP1Encoder() override = default;

    void paint(juce::Graphics& g) override;
    void setLabel(const juce::String& label) { labelText = label; repaint(); }

private:
    juce::Colour encoderColor;
    juce::String labelText;
};
