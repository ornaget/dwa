#pragma once
#include "EffectProcessor.h"

// Phone: Lo-fi / telephone / radio effect with filtering
class PhoneEffect : public EffectProcessor
{
public:
    PhoneEffect() = default;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void setParameter(int index, float value) override;
    juce::String getName() const override { return "Phone"; }
    juce::String getParameterName(int index) const override;

private:
    float locut = 0.5f;
    float hicut = 0.5f;
    float bitcrush = 0.0f;

    // Filter states
    float hpStateL[2] = {};
    float hpStateR[2] = {};
    float lpStateL[2] = {};
    float lpStateR[2] = {};

    // Sample-and-hold state (per-channel, persistent across buffer boundaries)
    float holdSampleL = 0.0f, holdSampleR = 0.0f;
    float holdCounterL = 0.0f, holdCounterR = 0.0f;
};
