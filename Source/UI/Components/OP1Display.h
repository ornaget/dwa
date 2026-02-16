#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../../Utils/OP1Colors.h"

// The main AMOLED-style display of the OP-1 Field
class OP1Display : public juce::Component
{
public:
    OP1Display();
    ~OP1Display() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // Content delegate - screens set this
    std::function<void(juce::Graphics&, juce::Rectangle<int>)> onPaint;

    // Header info
    void setModeName(const juce::String& name) { modeName = name; repaint(); }
    void setEngineName(const juce::String& name) { engineName = name; repaint(); }
    void setBatteryLevel(float level) { batteryLevel = level; repaint(); }
    void setTempo(float bpm) { tempo = bpm; repaint(); }

    // Parameter labels for encoders
    void setEncoderLabels(const juce::String& e1, const juce::String& e2,
                          const juce::String& e3, const juce::String& e4);
    void setEncoderValues(float v1, float v2, float v3, float v4);

private:
    juce::String modeName = "SYNTH";
    juce::String engineName = "FM";
    float batteryLevel = 0.85f;
    float tempo = 120.0f;

    juce::String encLabel[4] = { "P1", "P2", "P3", "P4" };
    float encValue[4] = { 0.5f, 0.5f, 0.5f, 0.5f };
    juce::Colour encColors[4] = { OP1Colors::encoderBlue, OP1Colors::encoderGreen,
                                   OP1Colors::encoderWhite, OP1Colors::encoderOrange };

    void drawHeader(juce::Graphics& g, juce::Rectangle<int> area);
    void drawEncoderBar(juce::Graphics& g, juce::Rectangle<int> area);
    void drawBattery(juce::Graphics& g, juce::Rectangle<int> area);
};
