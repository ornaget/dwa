#include "NitroEffect.h"

void NitroEffect::prepare(double sr, int spb)
{
    EffectProcessor::prepare(sr, spb);
    prevSampleL = 0.0f;
    prevSampleR = 0.0f;
}

void NitroEffect::process(juce::AudioBuffer<float>& buffer)
{
    float driveGain = 1.0f + drive * 20.0f;
    float toneCoeff = 0.1f + tone * 0.8f;

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        float& prevSample = (ch == 0) ? prevSampleL : prevSampleR;

        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float dry = data[i];

            // Drive with soft clipping (tanh saturation)
            float driven = std::tanh((dry + bias * 0.1f) * driveGain);

            // Asymmetric clipping for tube-like character
            if (driven > 0.0f)
                driven = 1.0f - std::exp(-driven);
            else
                driven = -(1.0f - std::exp(driven)) * 0.8f;

            // Tone filter (one-pole low-pass for darker sounds, bypass for bright)
            float filtered = driven * toneCoeff + prevSample * (1.0f - toneCoeff);
            prevSample = filtered;

            // Mix
            data[i] = dry * (1.0f - mix) + filtered * mix;
        }
    }
}

void NitroEffect::setParameter(int index, float value)
{
    switch (index)
    {
        case 0: drive = value; break;
        case 1: tone = value; break;
        case 2: bias = value; break;
    }
}

juce::String NitroEffect::getParameterName(int index) const
{
    switch (index)
    {
        case 0: return "Drive";
        case 1: return "Tone";
        case 2: return "Bias";
        default: return "";
    }
}
