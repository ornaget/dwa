#include "CWOEffect.h"

void CWOEffect::prepare(double sr, int spb)
{
    EffectProcessor::prepare(sr, spb);
    modPhase = 0.0f;
    prevFeedbackL = 0.0f;
    prevFeedbackR = 0.0f;
}

void CWOEffect::process(juce::AudioBuffer<float>& buffer)
{
    float modFreq = 0.1f + frequency * frequency * 5000.0f; // exponential frequency range
    float phaseInc = modFreq / (float)sampleRate;

    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        float mod = std::sin(modPhase * juce::MathConstants<float>::twoPi);
        float modQuad = std::cos(modPhase * juce::MathConstants<float>::twoPi); // quadrature for stereo

        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            float dry = data[i];

            float& prevFb = (ch == 0) ? prevFeedbackL : prevFeedbackR;
            float input = dry + prevFb * feedback;

            // Ring modulation / frequency shifting
            float modSignal = (ch == 0) ? mod : modQuad;
            float wet = input * (1.0f - depth) + input * modSignal * depth;

            prevFb = wet * 0.5f;

            data[i] = dry * (1.0f - mix) + wet * mix;
        }

        modPhase += phaseInc;
        if (modPhase >= 1.0f) modPhase -= 1.0f;
    }
}

void CWOEffect::setParameter(int index, float value)
{
    switch (index)
    {
        case 0: frequency = value; break;
        case 1: depth = value; break;
        case 2: feedback = value * 0.9f; break;
    }
}

juce::String CWOEffect::getParameterName(int index) const
{
    switch (index)
    {
        case 0: return "Freq";
        case 1: return "Depth";
        case 2: return "Feedback";
        default: return "";
    }
}
