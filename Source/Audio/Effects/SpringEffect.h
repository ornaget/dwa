#pragma once
#include "EffectProcessor.h"

// Spring reverb simulation
class SpringEffect : public EffectProcessor
{
public:
    SpringEffect() = default;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void setParameter(int index, float value) override;
    juce::String getName() const override { return "Spring"; }
    juce::String getParameterName(int index) const override;

private:
    float decay = 0.5f;
    float tone = 0.5f;
    float drip = 0.3f;

    // Multiple allpass delays for spring character
    static constexpr int numAllpass = 4;
    static constexpr int maxAllpassLen = 8192;

    struct AllpassSection
    {
        std::array<float, maxAllpassLen> buffer = {};
        int writePos = 0;
        int length = 1000;
        float feedback = 0.5f;
    };

    std::array<AllpassSection, numAllpass> allpassL;
    std::array<AllpassSection, numAllpass> allpassR;

    // Comb filters for density
    static constexpr int numComb = 4;
    static constexpr int maxCombLen = 16384;

    struct CombSection
    {
        std::array<float, maxCombLen> buffer = {};
        int writePos = 0;
        int length = 2000;
        float feedback = 0.7f;
        float filterState = 0.0f;
    };

    std::array<CombSection, numComb> combL;
    std::array<CombSection, numComb> combR;

    float processAllpass(AllpassSection& ap, float input);
    float processComb(CombSection& comb, float input, float damping);
};
