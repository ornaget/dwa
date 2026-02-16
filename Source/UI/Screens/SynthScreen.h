#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../../Utils/OP1Colors.h"

// Display content for Synth mode - visualizes the current synth engine
class SynthScreen
{
public:
    SynthScreen() = default;

    void paint(juce::Graphics& g, juce::Rectangle<int> bounds);

    void setWaveformData(const std::vector<float>& data) { waveformData = data; }
    void setEngineName(const juce::String& name) { engineName = name; }
    void setADSR(float a, float d, float s, float r) { attack = a; decay = d; sustain = s; release = r; }
    void setActiveNote(int note) { currentNote = note; }

private:
    std::vector<float> waveformData;
    juce::String engineName;
    float attack = 0.01f, decay = 0.3f, sustain = 0.7f, release = 0.5f;
    int currentNote = -1;

    void drawWaveform(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawADSR(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawNoteInfo(juce::Graphics& g, juce::Rectangle<int> bounds);
};
