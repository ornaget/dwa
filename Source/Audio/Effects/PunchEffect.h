#pragma once
#include "EffectProcessor.h"

// Punch: Compressor/transient shaper effect
class PunchEffect : public EffectProcessor
{
public:
    PunchEffect() = default;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void setParameter(int index, float value) override;
    juce::String getName() const override { return "Punch"; }
    juce::String getParameterName(int index) const override;

private:
    float punch = 0.5f;
    float squash = 0.5f;
    float attack = 0.5f;

    float envelopeL = 0.0f;
    float envelopeR = 0.0f;
};
