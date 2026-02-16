#pragma once
#include "SynthEngine.h"

// Cluster: multiple detuned oscillators creating thick pads
class ClusterSynth : public SynthEngine
{
public:
    ClusterSynth();
    ~ClusterSynth() override = default;

    void prepare(double sampleRate, int samplesPerBlock) override;
    void noteOn(int midiNote, float velocity) override;
    void noteOff(int midiNote) override;
    void renderBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) override;
    void setParameter(int index, float value) override;
    juce::String getName() const override { return "Cluster"; }
    juce::String getParameterName(int index) const override;

private:
    static constexpr int maxSubOsc = 8;

    struct ClusterVoice : Voice
    {
        float subPhases[maxSubOsc] = {};
        float subFreqs[maxSubOsc] = {};
    };

    std::array<ClusterVoice, maxVoices> voices;

    float spread = 0.3f;
    int numVoicesParam = 4;
    float detuneAmount = 0.2f;
    float shapeParam = 0.0f;

    float generateOsc(float phase, float shape);
};
