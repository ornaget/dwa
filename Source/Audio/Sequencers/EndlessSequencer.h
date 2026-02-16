#pragma once
#include "SequencerBase.h"

// Endless: Loop-based sequencer that keeps recording and layering
class EndlessSequencer : public SequencerBase
{
public:
    EndlessSequencer();
    void processBlock(juce::MidiBuffer& midiOut, int numSamples) override;
    juce::String getName() const override { return "Endless"; }
    int getNumSteps() const override { return numSteps; }

    void addNote(int step, int midiNote, float velocity);
    void clearStep(int step);
    void clearAll();
    void setNumSteps(int steps);

    const NoteEvent& getStep(int step) const { return steps[(size_t)juce::jlimit(0, maxSteps - 1, step)]; }

    void reset() override { SequencerBase::reset(); firstStepPending = true; }

private:
    static constexpr int maxSteps = 128;
    int numSteps = 16;
    std::array<NoteEvent, maxSteps> steps;
    int activeNoteOff = -1;
    int noteOffCountdown = 0;
    bool firstStepPending = true;
};
