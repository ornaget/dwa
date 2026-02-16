#include "SpringEffect.h"

void SpringEffect::prepare(double sr, int spb)
{
    EffectProcessor::prepare(sr, spb);

    // Set up allpass delay lengths (prime numbers for diffusion)
    int allpassLens[] = { 113, 337, 547, 809 };
    for (int i = 0; i < numAllpass; ++i)
    {
        int len = (int)(allpassLens[i] * sr / 44100.0);
        allpassL[i].length = juce::jlimit(1, maxAllpassLen - 1, len);
        allpassR[i].length = juce::jlimit(1, maxAllpassLen - 1, len + 23);
        allpassL[i].buffer.fill(0.0f);
        allpassR[i].buffer.fill(0.0f);
        allpassL[i].writePos = 0;
        allpassR[i].writePos = 0;
        allpassL[i].feedback = 0.5f;
        allpassR[i].feedback = 0.5f;
    }

    int combLens[] = { 1687, 1901, 2143, 2473 };
    for (int i = 0; i < numComb; ++i)
    {
        int len = (int)(combLens[i] * sr / 44100.0);
        combL[i].length = juce::jlimit(1, maxCombLen - 1, len);
        combR[i].length = juce::jlimit(1, maxCombLen - 1, len + 37);
        combL[i].buffer.fill(0.0f);
        combR[i].buffer.fill(0.0f);
        combL[i].writePos = 0;
        combR[i].writePos = 0;
        combL[i].filterState = 0.0f;
        combR[i].filterState = 0.0f;
    }
}

float SpringEffect::processAllpass(AllpassSection& ap, float input)
{
    int readPos = ap.writePos - ap.length;
    if (readPos < 0) readPos += maxAllpassLen;

    float delayed = ap.buffer[(size_t)readPos];
    float output = -input + delayed;
    ap.buffer[(size_t)ap.writePos] = input + delayed * ap.feedback;

    ap.writePos++;
    if (ap.writePos >= maxAllpassLen) ap.writePos = 0;

    return output;
}

float SpringEffect::processComb(CombSection& comb, float input, float damping)
{
    int readPos = comb.writePos - comb.length;
    if (readPos < 0) readPos += maxCombLen;

    float delayed = comb.buffer[(size_t)readPos];

    // Low-pass filter in feedback path for spring character
    comb.filterState = delayed * (1.0f - damping) + comb.filterState * damping;

    float fbAmount = decay * 0.85f;
    comb.buffer[(size_t)comb.writePos] = input + comb.filterState * fbAmount;

    comb.writePos++;
    if (comb.writePos >= maxCombLen) comb.writePos = 0;

    return delayed;
}

void SpringEffect::process(juce::AudioBuffer<float>& buffer)
{
    float damping = 1.0f - tone;

    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            float dry = data[i];
            float input = dry;

            auto& allpasses = (ch == 0) ? allpassL : allpassR;
            auto& combs = (ch == 0) ? combL : combR;

            // Pre-diffusion through allpass chain
            float diffused = input;
            for (int a = 0; a < numAllpass; ++a)
                diffused = processAllpass(allpasses[a], diffused);

            // Parallel comb filters
            float combOut = 0.0f;
            for (int c = 0; c < numComb; ++c)
                combOut += processComb(combs[c], diffused, damping);
            combOut /= (float)numComb;

            // Spring "drip" effect - subtle pitch modulation
            if (drip > 0.01f)
            {
                float dripMod = std::sin((float)i / (float)buffer.getNumSamples()
                                         * juce::MathConstants<float>::twoPi * 3.0f);
                combOut += combOut * dripMod * drip * 0.1f;
            }

            data[i] = dry * (1.0f - mix) + combOut * mix;
        }
    }
}

void SpringEffect::setParameter(int index, float value)
{
    switch (index)
    {
        case 0: decay = value; break;
        case 1: tone = value; break;
        case 2: drip = value; break;
    }
}

juce::String SpringEffect::getParameterName(int index) const
{
    switch (index)
    {
        case 0: return "Decay";
        case 1: return "Tone";
        case 2: return "Drip";
        default: return "";
    }
}
