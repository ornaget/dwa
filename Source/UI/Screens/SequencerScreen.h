#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../../Utils/OP1Colors.h"

class SequencerScreen
{
public:
    SequencerScreen() = default;
    void paint(juce::Graphics& g, juce::Rectangle<int> bounds);

    void setSequencerName(const juce::String& name) { seqName = name; }
    void setNumSteps(int steps) { numSteps = steps; }
    void setCurrentStep(int step) { currentStep = step; }
    void setStepActive(int step, bool active);
    void setStepNote(int step, int note);
    void setPlaying(bool p) { isPlaying = p; }

private:
    juce::String seqName = "Endless";
    int numSteps = 16;
    int currentStep = 0;
    bool isPlaying = false;

    struct StepInfo {
        bool active = false;
        int midiNote = 60;
    };
    std::array<StepInfo, 128> steps;

    void drawStepGrid(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawPianoRoll(juce::Graphics& g, juce::Rectangle<int> bounds);
};
