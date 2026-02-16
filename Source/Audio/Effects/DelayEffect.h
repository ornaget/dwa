#pragma once
#include "EffectProcessor.h"

class DelayEffect : public EffectProcessor
{
public:
    DelayEffect() = default;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void setParameter(int index, float value) override;
    juce::String getName() const override { return "Delay"; }
    juce::String getParameterName(int index) const override;

private:
    float time = 0.3f;
    float feedback = 0.4f;
    float tone = 0.5f;

    static constexpr int maxDelaySamples = 88200; // 2 sec at 44100
    std::array<float, maxDelaySamples> delayBufferL = {};
    std::array<float, maxDelaySamples> delayBufferR = {};
    int writePos = 0;
    float prevFilterL = 0.0f;
    float prevFilterR = 0.0f;
};
