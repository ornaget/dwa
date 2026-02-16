#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../../Utils/OP1Colors.h"

class VUMeter : public juce::Component, public juce::Timer
{
public:
    VUMeter();
    ~VUMeter() override = default;

    void paint(juce::Graphics& g) override;
    void timerCallback() override;

    void setLevel(float left, float right);

private:
    float levelL = 0.0f;
    float levelR = 0.0f;
    float peakL = 0.0f;
    float peakR = 0.0f;
    float peakHoldL = 0.0f;
    float peakHoldR = 0.0f;
    int peakHoldCounterL = 0;
    int peakHoldCounterR = 0;

    void drawChannel(juce::Graphics& g, juce::Rectangle<float> bounds, float level, float peak);
};
