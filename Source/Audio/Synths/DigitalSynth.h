#pragma once
#include "SynthEngine.h"

class DigitalSynth : public SynthEngine
{
public:
    DigitalSynth();
    ~DigitalSynth() override = default;

    void prepare(double sampleRate, int samplesPerBlock) override;
    void noteOn(int midiNote, float velocity) override;
    void noteOff(int midiNote) override;
    void renderBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) override;
    void setParameter(int index, float value) override;
    juce::String getName() const override { return "Digital"; }
    juce::String getParameterName(int index) const override;

private:
    struct DigiVoice : Voice
    {
        float phase2 = 0.0f;
    };

    std::array<DigiVoice, maxVoices> voices;

    float waveform = 0.0f;   // morph between wave shapes
    float detune = 0.0f;     // second oscillator detune
    float phaseOffset = 0.0f;
    float fold = 0.0f;       // wavefolder amount

    float generateWaveform(float phase, float wf);
    float applyFold(float sample, float foldAmount);
};
