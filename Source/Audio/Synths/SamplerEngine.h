#pragma once
#include "SynthEngine.h"
#include <atomic>

// Simple sampler that can record and playback audio
class SamplerEngine : public SynthEngine
{
public:
    SamplerEngine();
    ~SamplerEngine() override = default;

    void prepare(double sampleRate, int samplesPerBlock) override;
    void noteOn(int midiNote, float velocity) override;
    void noteOff(int midiNote) override;
    void renderBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) override;
    void setParameter(int index, float value) override;
    juce::String getName() const override { return "Sampler"; }
    juce::String getParameterName(int index) const override;

    void recordSample(const float* data, int numSamples);
    bool isRecording() const { return recording.load(); }
    void startRecording();
    void stopRecording();

private:
    struct SamplerVoice : Voice
    {
        double playbackPos = 0.0; // double for precision with long samples
        float playbackRate = 1.0f;
        bool playing = false;
    };

    std::array<SamplerVoice, maxVoices> voices;
    std::vector<float> sampleBuffer;
    int sampleLength = 0;
    int baseNote = 60; // middle C

    float startPos = 0.0f;
    float endPos = 1.0f;
    float loopAmount = 0.0f;
    float grainSize = 0.0f;

    std::atomic<bool> recording { false };
    std::atomic<int> recordPos { 0 };
    static constexpr int maxRecordLength = 44100 * 30; // 30 seconds max
};
