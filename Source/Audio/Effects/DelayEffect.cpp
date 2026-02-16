#include "DelayEffect.h"

void DelayEffect::prepare(double sr, int spb)
{
    EffectProcessor::prepare(sr, spb);
    delayBufferL.fill(0.0f);
    delayBufferR.fill(0.0f);
    writePos = 0;
    prevFilterL = 0.0f;
    prevFilterR = 0.0f;
}

void DelayEffect::process(juce::AudioBuffer<float>& buffer)
{
    int delaySamples = juce::jlimit(1, maxDelaySamples - 1,
                                     (int)(time * time * (float)sampleRate * 2.0f));
    float toneCoeff = 0.3f + tone * 0.6f;

    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        int readPos = writePos - delaySamples;
        if (readPos < 0) readPos += maxDelaySamples;

        float delayedL = delayBufferL[(size_t)readPos];
        float delayedR = delayBufferR[(size_t)readPos];

        // Tone filter on feedback path
        delayedL = delayedL * toneCoeff + prevFilterL * (1.0f - toneCoeff);
        delayedR = delayedR * toneCoeff + prevFilterR * (1.0f - toneCoeff);
        prevFilterL = delayedL;
        prevFilterR = delayedR;

        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            float dry = data[i];
            float delayed = (ch == 0) ? delayedL : delayedR;

            // Write to delay buffer with feedback
            if (ch == 0)
                delayBufferL[(size_t)writePos] = dry + delayed * feedback;
            else
                delayBufferR[(size_t)writePos] = dry + delayed * feedback;

            // Cross-feed for stereo width (ping-pong like)
            float crossFeed = (ch == 0) ? delayedR * 0.2f : delayedL * 0.2f;

            data[i] = dry * (1.0f - mix) + (delayed + crossFeed) * mix;
        }

        writePos++;
        if (writePos >= maxDelaySamples) writePos = 0;
    }
}

void DelayEffect::setParameter(int index, float value)
{
    switch (index)
    {
        case 0: time = value; break;
        case 1: feedback = value * 0.95f; break;
        case 2: tone = value; break;
    }
}

juce::String DelayEffect::getParameterName(int index) const
{
    switch (index)
    {
        case 0: return "Time";
        case 1: return "Feedback";
        case 2: return "Tone";
        default: return "";
    }
}
