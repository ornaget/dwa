#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class EffectProcessor
{
public:
    EffectProcessor() = default;
    virtual ~EffectProcessor() = default;

    virtual void prepare(double sampleRate, int samplesPerBlock)
    {
        this->sampleRate = sampleRate;
        this->blockSize = samplesPerBlock;
    }

    virtual void process(juce::AudioBuffer<float>& buffer) = 0;
    virtual void setParameter(int index, float value) { juce::ignoreUnused(index, value); }
    virtual juce::String getName() const = 0;
    virtual juce::String getParameterName(int index) const
    {
        juce::ignoreUnused(index);
        return "Param";
    }

    void setMix(float newMix) { mix = newMix; }

protected:
    double sampleRate = 44100.0;
    int blockSize = 512;
    float mix = 0.3f;
};
