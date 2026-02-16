#pragma once
#include "SequencerBase.h"

// Sketch: Draw-style sequencer where you sketch note patterns
class SketchSequencer : public SequencerBase
{
public:
    SketchSequencer();
    void processBlock(juce::MidiBuffer& midiOut, int numSamples) override;
    juce::String getName() const override { return "Sketch"; }
    int getNumSteps() const override { return numSteps; }

    // Sketch a note value at a position (like drawing on screen)
    void setNoteAtStep(int step, int midiNote);
    void setVelocityAtStep(int step, float velocity);
    void clearStep(int step);
    void clearAll();
    void setNumSteps(int steps);

    int getNoteAtStep(int step) const;
    float getVelocityAtStep(int step) const;
    bool isStepActive(int step) const;

private:
    static constexpr int maxSteps = 64;
    int numSteps = 32;
    std::array<NoteEvent, maxSteps> steps;
    int activeNoteOff = -1;
    int noteOffCountdown = 0;
};
