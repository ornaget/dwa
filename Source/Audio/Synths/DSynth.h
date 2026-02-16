#pragma once
#include "SynthEngine.h"

// DSynth: Digital synthesis with multiple algorithms
class DSynth : public SynthEngine
{
public:
    DSynth();
    ~DSynth() override = default;

    void prepare(double sampleRate, int samplesPerBlock) override;
    void noteOn(int midiNote, float velocity) override;
    void noteOff(int midiNote) override;
    void renderBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) override;
    void setParameter(int index, float value) override;
    juce::String getName() const override { return "DSynth"; }
    juce::String getParameterName(int index) const override;

private:
    struct DSynthVoice : Voice
    {
        float phase2 = 0.0f;
        float phase3 = 0.0f;
        float accumulator = 0.0f;
    };

    std::array<DSynthVoice, maxVoices> voices;

    int algorithm = 0;
    float param1 = 0.5f;
    float param2 = 0.5f;
    float param3 = 0.5f;

    float runAlgorithm(DSynthVoice& v, float freq);
};
