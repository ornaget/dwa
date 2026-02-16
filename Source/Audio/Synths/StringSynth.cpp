#include "StringSynth.h"

StringSynth::StringSynth()
{
    for (auto& v : voices) v.active = false;
}

void StringSynth::prepare(double sampleRate, int spb)
{
    SynthEngine::prepare(sampleRate, spb);
    for (auto& v : voices)
    {
        v.delayLine.resize((int)(sampleRate / 20.0), 0.0f); // enough for lowest freq
        v.adsr.setSampleRate(sampleRate);
        v.active = false;
    }
}

void StringSynth::noteOn(int midiNote, float velocity)
{
    StringVoice* target = nullptr;
    for (auto& v : voices)
        if (!v.active) { target = &v; break; }
    if (!target) target = &voices[0];

    target->midiNote = midiNote;
    target->frequency = midiNoteToFreq(midiNote);
    target->velocity = velocity;
    target->active = true;
    target->excitation = 1.0f;
    target->prevSample = 0.0f;
    target->delayWritePos = 0;
    target->delayLength = (int)(currentSampleRate / target->frequency);
    if (target->delayLength < 2) target->delayLength = 2;
    if (target->delayLength > (int)target->delayLine.size())
        target->delayLength = (int)target->delayLine.size();

    // Fill delay line with noise burst (pluck excitation)
    juce::Random rng;
    for (int i = 0; i < target->delayLength; ++i)
    {
        float noise = rng.nextFloat() * 2.0f - 1.0f;
        // Shape the burst based on pluck parameter
        float envelope = 1.0f;
        if (pluck < 0.5f)
        {
            // Softer pluck - low-pass the noise
            float t = (float)i / (float)target->delayLength;
            envelope = std::sin(t * juce::MathConstants<float>::pi);
        }
        target->delayLine[(size_t)i] = noise * velocity * envelope;
    }

    target->adsr.setSampleRate(currentSampleRate);
    auto p = adsrParams;
    p.sustain = 0.0f; // String naturally decays
    p.release = 0.01f;
    target->adsr.setParameters(p);
    target->adsr.noteOn();
}

void StringSynth::noteOff(int midiNote)
{
    for (auto& v : voices)
        if (v.active && v.midiNote == midiNote)
            v.adsr.noteOff();
}

void StringSynth::renderBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    float chorusRate = 0.5f;
    float chorusDepth = chorusAmount * 3.0f;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float monoSample = 0.0f;
        chorusPhase += chorusRate / (float)currentSampleRate;
        if (chorusPhase >= 1.0f) chorusPhase -= 1.0f;

        for (auto& v : voices)
        {
            if (!v.active) continue;

            // Read from delay line with interpolation
            int readPos = v.delayWritePos - v.delayLength;
            if (readPos < 0) readPos += (int)v.delayLine.size();

            float current = v.delayLine[(size_t)readPos];

            // Karplus-Strong: average with previous sample (low-pass filter)
            float dampCoeff = 0.5f + damping * 0.48f;
            float filtered = current * (1.0f - dampCoeff) + v.prevSample * dampCoeff;
            v.prevSample = filtered;

            // Body resonance (simple second-order filter effect)
            float bodyGain = 0.8f + body * 0.2f;
            float output = filtered * bodyGain;

            // Chorus modulation
            if (chorusAmount > 0.01f)
            {
                int size = (int)v.delayLine.size();
                float modOffset = std::sin(chorusPhase * juce::MathConstants<float>::twoPi) * chorusDepth;
                int modReadPos = readPos - (int)modOffset;
                modReadPos = ((modReadPos % size) + size) % size; // Safe wrapping
                float chorusSample = v.delayLine[(size_t)modReadPos] * 0.3f;
                output += chorusSample * chorusAmount;
            }

            // Write back to delay line (sustain the resonance)
            float decayFactor = 0.996f - damping * 0.01f;
            v.delayLine[(size_t)v.delayWritePos] = filtered * decayFactor;
            v.delayWritePos++;
            if (v.delayWritePos >= (int)v.delayLine.size())
                v.delayWritePos = 0;

            // Decay the excitation state
            v.excitation *= 0.999f;

            // Check if string has died out
            if (std::abs(output) < 0.0001f && v.excitation < 0.001f)
            {
                v.active = false;
                continue;
            }

            monoSample += output * v.velocity;
        }

        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            buffer.addSample(ch, startSample + sample, monoSample * 0.4f);
    }
}

void StringSynth::setParameter(int index, float value)
{
    switch (index)
    {
        case 0: damping = value; break;
        case 1: body = value; break;
        case 2: pluck = value; break;
        case 3: chorusAmount = value; break;
    }
}

juce::String StringSynth::getParameterName(int index) const
{
    switch (index)
    {
        case 0: return "Damping";
        case 1: return "Body";
        case 2: return "Pluck";
        case 3: return "Chorus";
        default: return "";
    }
}
