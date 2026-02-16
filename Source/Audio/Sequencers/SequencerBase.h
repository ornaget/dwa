#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <atomic>

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

    void setTempo(float bpm) { tempo.store(juce::jmax(1.0f, bpm)); }
    float getTempo() const { return tempo.load(); }

    void setPlaying(bool play) { isPlaying.store(play); if (!play) reset(); }
    bool getPlaying() const { return isPlaying.load(); }

    int getCurrentStep() const { return currentStep; }
    virtual int getNumSteps() const { return 16; }

protected:
    double sampleRate = 44100.0;
    std::atomic<float> tempo { 120.0f };
    int currentStep = 0;
    int sampleCounter = 0;
    std::atomic<bool> isPlaying { false };

    int getSamplesPerStep() const
    {
        float t = tempo.load();
        if (t <= 0.0f) t = 120.0f; // Prevent division by zero
        return juce::jmax(1, (int)(sampleRate * 60.0 / (double)t / 4.0));
    }
};
