#pragma once
#include "SequencerBase.h"

// Pattern: Traditional step sequencer with patterns
class PatternSequencer : public SequencerBase
{
public:
    PatternSequencer();
    void processBlock(juce::MidiBuffer& midiOut, int numSamples) override;
    juce::String getName() const override { return "Pattern"; }
    int getNumSteps() const override { return 16; }

    void toggleStep(int step, int midiNote, float velocity);
    void setPattern(int patternIndex);
    int getCurrentPattern() const { return currentPattern; }

    bool isStepActive(int pattern, int step) const;
    const NoteEvent& getStep(int pattern, int step) const;

private:
    static constexpr int numPatterns = 8;
    static constexpr int stepsPerPattern = 16;

    struct Pattern
    {
        std::array<NoteEvent, stepsPerPattern> steps;
    };

    std::array<Pattern, numPatterns> patterns;
    int currentPattern = 0;
    int activeNoteOff = -1;
    int noteOffCountdown = 0;
};
