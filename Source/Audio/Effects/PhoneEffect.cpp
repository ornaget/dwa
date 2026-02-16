#include "PhoneEffect.h"

void PhoneEffect::prepare(double sr, int spb)
{
    EffectProcessor::prepare(sr, spb);
    std::memset(hpStateL, 0, sizeof(hpStateL));
    std::memset(hpStateR, 0, sizeof(hpStateR));
    std::memset(lpStateL, 0, sizeof(lpStateL));
    std::memset(lpStateR, 0, sizeof(lpStateR));
}

void PhoneEffect::process(juce::AudioBuffer<float>& buffer)
{
    // High-pass frequency: 100 Hz to 2000 Hz
    float hpFreq = 100.0f + locut * locut * 1900.0f;
    float hpCoeff = 1.0f - std::exp(-juce::MathConstants<float>::twoPi * hpFreq / (float)sampleRate);

    // Low-pass frequency: 1000 Hz to 20000 Hz
    float lpFreq = 1000.0f + (1.0f - hicut) * 19000.0f;
    float lpCoeff = 1.0f - std::exp(-juce::MathConstants<float>::twoPi * lpFreq / (float)sampleRate);

    // Bitcrush
    int bits = 16 - (int)(bitcrush * 12.0f);
    float levels = (float)(1 << bits);
    float sampleRateReduce = 1.0f + bitcrush * 20.0f;

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        float* hpState = (ch == 0) ? hpStateL : hpStateR;
        float* lpState = (ch == 0) ? lpStateL : lpStateR;

        float holdSample = 0.0f;
        float holdCounter = 0.0f;

        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float dry = data[i];
            float wet = dry;

            // High-pass filter
            hpState[0] += hpCoeff * (wet - hpState[0]);
            wet = wet - hpState[0];

            // Low-pass filter
            lpState[0] += lpCoeff * (wet - lpState[0]);
            wet = lpState[0];

            // Bit crushing
            if (bitcrush > 0.01f)
            {
                wet = std::round(wet * levels) / levels;

                // Sample rate reduction
                holdCounter += 1.0f;
                if (holdCounter >= sampleRateReduce)
                {
                    holdCounter -= sampleRateReduce;
                    holdSample = wet;
                }
                wet = holdSample;
            }

            // Subtle saturation
            wet = std::tanh(wet * 1.5f);

            data[i] = dry * (1.0f - mix) + wet * mix;
        }
    }
}

void PhoneEffect::setParameter(int index, float value)
{
    switch (index)
    {
        case 0: locut = value; break;
        case 1: hicut = value; break;
        case 2: bitcrush = value; break;
    }
}

juce::String PhoneEffect::getParameterName(int index) const
{
    switch (index)
    {
        case 0: return "Lo Cut";
        case 1: return "Hi Cut";
        case 2: return "Crush";
        default: return "";
    }
}
