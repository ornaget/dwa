#pragma once
#include "SynthEngine.h"

// Synthesized drum sounds (kick, snare, hihat, etc.)
class DrumSampler : public SynthEngine
{
public:
    DrumSampler();
    ~DrumSampler() override = default;

    void prepare(double sampleRate, int samplesPerBlock) override;
    void noteOn(int midiNote, float velocity) override;
    void noteOff(int midiNote) override;
    void renderBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) override;
    void setParameter(int index, float value) override;
    juce::String getName() const override { return "Drum"; }
    juce::String getParameterName(int index) const override;

private:
    struct DrumVoice
    {
        int type = 0; // 0=kick, 1=snare, 2=hihat, 3=clap, etc.
        float phase = 0.0f;
        float frequency = 0.0f;
        float velocity = 0.0f;
        float samplePos = 0.0f;
        float pitchEnv = 1.0f;
        float ampEnv = 1.0f;
        bool active = false;
    };

    static constexpr int numDrumVoices = 16;
    std::array<DrumVoice, numDrumVoices> drumVoices;

    float tone = 0.5f;
    float decay = 0.5f;
    float snap = 0.5f;
    float distortion = 0.0f;

    float generateKick(DrumVoice& v);
    float generateSnare(DrumVoice& v);
    float generateHihat(DrumVoice& v);
    float generateClap(DrumVoice& v);
    float generateTom(DrumVoice& v);
    float generateRim(DrumVoice& v);
    float generateCowbell(DrumVoice& v);
    float generateCymbal(DrumVoice& v);
};
