#pragma once
#include "SynthEngine.h"

// Karplus-Strong inspired string synthesis
class StringSynth : public SynthEngine
{
public:
    StringSynth();
    ~StringSynth() override = default;

    void prepare(double sampleRate, int samplesPerBlock) override;
    void noteOn(int midiNote, float velocity) override;
    void noteOff(int midiNote) override;
    void renderBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) override;
    void setParameter(int index, float value) override;
    juce::String getName() const override { return "String"; }
    juce::String getParameterName(int index) const override;

private:
    struct StringVoice : Voice
    {
        std::vector<float> delayLine;
        int delayWritePos = 0;
        int delayLength = 0;
        float prevSample = 0.0f;
        float excitation = 1.0f;
    };

    std::array<StringVoice, maxVoices> voices;

    float damping = 0.5f;
    float body = 0.5f;
    float pluck = 0.5f;
    float chorusAmount = 0.3f;
    float chorusPhase = 0.0f;
};
