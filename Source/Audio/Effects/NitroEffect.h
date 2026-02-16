#pragma once
#include "EffectProcessor.h"

// Nitro: Overdrive/distortion effect
class NitroEffect : public EffectProcessor
{
public:
    NitroEffect() = default;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void setParameter(int index, float value) override;
    juce::String getName() const override { return "Nitro"; }
    juce::String getParameterName(int index) const override;

private:
    float drive = 0.5f;
    float tone = 0.5f;
    float bias = 0.0f;

    // Simple one-pole filter for tone control
    float prevSampleL = 0.0f;
    float prevSampleR = 0.0f;
};
