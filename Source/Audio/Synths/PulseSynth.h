#pragma once
#include "SynthEngine.h"

class PulseSynth : public SynthEngine
{
public:
    PulseSynth();
    ~PulseSynth() override = default;

    void prepare(double sampleRate, int samplesPerBlock) override;
    void noteOn(int midiNote, float velocity) override;
    void noteOff(int midiNote) override;
    void renderBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) override;
    void setParameter(int index, float value) override;
    juce::String getName() const override { return "Pulse"; }
    juce::String getParameterName(int index) const override;

private:
    struct PulseVoice : Voice
    {
        float syncPhase = 0.0f;
    };

    std::array<PulseVoice, maxVoices> voices;

    float pulseWidth = 0.5f;
    float fineTune = 0.0f;
    float shapeAmount = 0.0f;
    float syncAmount = 0.0f;
};
