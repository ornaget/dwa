#include "PunchEffect.h"

void PunchEffect::prepare(double sr, int spb)
{
    EffectProcessor::prepare(sr, spb);
    envelopeL = 0.0f;
    envelopeR = 0.0f;
}

void PunchEffect::process(juce::AudioBuffer<float>& buffer)
{
    float attackCoeff = std::exp(-1.0f / (float)(sampleRate * (0.001f + attack * 0.05f)));
    float releaseCoeff = std::exp(-1.0f / (float)(sampleRate * (0.01f + (1.0f - attack) * 0.2f)));

    float threshold = 1.0f - squash * 0.8f;
    float ratio = 1.0f + squash * 7.0f;

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        float& envelope = (ch == 0) ? envelopeL : envelopeR;

        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float dry = data[i];
            float absInput = std::abs(dry);

            // Envelope follower
            if (absInput > envelope)
                envelope = attackCoeff * envelope + (1.0f - attackCoeff) * absInput;
            else
                envelope = releaseCoeff * envelope + (1.0f - releaseCoeff) * absInput;

            // Compression
            float gain = 1.0f;
            if (envelope > threshold)
            {
                float excess = envelope - threshold;
                float compressed = threshold + excess / ratio;
                gain = compressed / (envelope + 0.0001f);
            }

            // Transient enhancement (punch)
            float transient = absInput - envelope;
            if (transient > 0.0f)
                gain += transient * punch * 3.0f;

            float wet = dry * gain;

            // Soft clip
            wet = std::tanh(wet);

            data[i] = dry * (1.0f - mix) + wet * mix;
        }
    }
}

void PunchEffect::setParameter(int index, float value)
{
    switch (index)
    {
        case 0: punch = value; break;
        case 1: squash = value; break;
        case 2: attack = value; break;
    }
}

juce::String PunchEffect::getParameterName(int index) const
{
    switch (index)
    {
        case 0: return "Punch";
        case 1: return "Squash";
        case 2: return "Attack";
        default: return "";
    }
}
