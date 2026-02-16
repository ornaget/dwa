#pragma once
#include "EffectProcessor.h"

// CWO: Frequency shifting / ring modulation effect (like OP-1's CWO)
class CWOEffect : public EffectProcessor
{
public:
    CWOEffect() = default;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void setParameter(int index, float value) override;
    juce::String getName() const override { return "CWO"; }
    juce::String getParameterName(int index) const override;

private:
    float frequency = 0.5f;
    float depth = 0.5f;
    float feedback = 0.0f;

    float modPhase = 0.0f;
    float prevFeedbackL = 0.0f;
    float prevFeedbackR = 0.0f;
};
