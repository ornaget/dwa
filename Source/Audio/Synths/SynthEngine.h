#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class SynthEngine
{
public:
    SynthEngine() = default;
    virtual ~SynthEngine() = default;

    virtual void prepare(double sampleRate, int samplesPerBlock)
    {
        currentSampleRate = sampleRate;
        this->samplesPerBlock = samplesPerBlock;
    }

    virtual void noteOn(int midiNote, float velocity) = 0;
    virtual void noteOff(int midiNote) = 0;
    virtual void renderBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) = 0;

    virtual void setParameter(int index, float value) { juce::ignoreUnused(index, value); }
    virtual juce::String getName() const = 0;
    virtual juce::String getParameterName(int index) const
    {
        juce::ignoreUnused(index);
        return "Param";
    }

    void setADSR(float attack, float decay, float sustain, float release)
    {
        adsrParams.attack = attack;
        adsrParams.decay = decay;
        adsrParams.sustain = sustain;
        adsrParams.release = release;
    }

protected:
    double currentSampleRate = 44100.0;
    int samplesPerBlock = 512;
    juce::ADSR::Parameters adsrParams { 0.01f, 0.3f, 0.7f, 0.5f };

    // Polyphonic voice support
    struct Voice
    {
        int midiNote = -1;
        float frequency = 0.0f;
        float velocity = 0.0f;
        float phase = 0.0f;
        juce::ADSR adsr;
        bool active = false;
    };

    static constexpr int maxVoices = 8;

    float midiNoteToFreq(int note) const
    {
        return 440.0f * std::pow(2.0f, (note - 69) / 12.0f);
    }
};
