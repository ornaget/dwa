#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

class SequencerBase
{
public:
    struct NoteEvent
    {
        int midiNote = 60;
        float velocity = 0.8f;
        float duration = 0.25f; // in beats
        bool active = false;
    };

    SequencerBase() = default;
    virtual ~SequencerBase() = default;

    virtual void prepare(double sampleRate) { this->sampleRate = sampleRate; }
    virtual void reset() { currentStep = 0; sampleCounter = 0; }

    // Process and return MIDI events for the current block
    virtual void processBlock(juce::MidiBuffer& midiOut, int numSamples) = 0;

    virtual juce::String getName() const = 0;

    void setTempo(float bpm) { tempo = bpm; }
    float getTempo() const { return tempo; }

    void setPlaying(bool play) { isPlaying = play; if (!play) reset(); }
    bool getPlaying() const { return isPlaying; }

    int getCurrentStep() const { return currentStep; }
    virtual int getNumSteps() const { return 16; }

protected:
    double sampleRate = 44100.0;
    float tempo = 120.0f;
    int currentStep = 0;
    int sampleCounter = 0;
    bool isPlaying = false;

    int getSamplesPerStep() const
    {
        // 16th notes by default
        return (int)(sampleRate * 60.0 / tempo / 4.0);
    }
};
