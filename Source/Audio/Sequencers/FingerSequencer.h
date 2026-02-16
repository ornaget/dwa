#pragma once
#include "SequencerBase.h"

// Finger: Records played notes in real-time and loops them
class FingerSequencer : public SequencerBase
{
public:
    FingerSequencer();
    void processBlock(juce::MidiBuffer& midiOut, int numSamples) override;
    juce::String getName() const override { return "Finger"; }
    int getNumSteps() const override { return loopLengthSteps; }

    void recordNoteOn(int midiNote, float velocity);
    void recordNoteOff(int midiNote);
    void startRecording();
    void stopRecording();
    void clearRecording();
    bool isRecordingActive() const { return isRecording; }

private:
    struct RecordedEvent
    {
        int midiNote = 0;
        float velocity = 0.0f;
        int samplePosition = 0;
        bool isNoteOn = true;
    };

    std::vector<RecordedEvent> recordedEvents;
    int recordStartSample = 0;
    int loopLengthSamples = 0;
    int loopLengthSteps = 16;
    int playbackPosition = 0;
    bool isRecording = false;

    static constexpr int maxEvents = 1024;
};
