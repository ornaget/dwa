#pragma once
#include "SynthEngine.h"

class FMSynth : public SynthEngine
{
public:
    FMSynth();
    ~FMSynth() override = default;

    void prepare(double sampleRate, int samplesPerBlock) override;
    void noteOn(int midiNote, float velocity) override;
    void noteOff(int midiNote) override;
    void renderBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) override;
    void setParameter(int index, float value) override;
    juce::String getName() const override { return "FM"; }
    juce::String getParameterName(int index) const override;

private:
    struct FMVoice : Voice
    {
        float modPhase = 0.0f;
        float lastModOutput = 0.0f; // Per-voice feedback state
    };

    std::array<FMVoice, maxVoices> voices;

    float ratio = 2.0f;
    float depth = 0.3f;
    float feedback = 0.0f;
    float shape = 0.0f;      // morphs carrier waveform

    float generateCarrier(float phase, float shapeAmount);
};
