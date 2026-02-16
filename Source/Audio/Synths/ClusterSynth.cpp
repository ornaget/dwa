#include "ClusterSynth.h"

ClusterSynth::ClusterSynth()
{
    for (auto& v : voices) v.active = false;
}

void ClusterSynth::prepare(double sampleRate, int spb)
{
    SynthEngine::prepare(sampleRate, spb);
    for (auto& v : voices)
    {
        v.adsr.setSampleRate(sampleRate);
        v.active = false;
    }
}

void ClusterSynth::noteOn(int midiNote, float velocity)
{
    ClusterVoice* target = nullptr;
    for (auto& v : voices)
        if (!v.active) { target = &v; break; }
    if (!target) target = &voices[0];

    target->midiNote = midiNote;
    target->frequency = midiNoteToFreq(midiNote);
    target->velocity = velocity;
    target->active = true;

    // Set up sub-oscillators with spread detuning
    juce::Random rng(midiNote * 31 + 7);
    for (int i = 0; i < maxSubOsc; ++i)
    {
        float detuneOffset = (float)(i - numVoicesParam / 2) / (float)numVoicesParam;
        float randomSpread = (rng.nextFloat() - 0.5f) * spread * 0.1f;
        target->subFreqs[i] = target->frequency * (1.0f + detuneOffset * detuneAmount * 0.05f + randomSpread);
        target->subPhases[i] = rng.nextFloat(); // random start phase for richness
    }

    target->adsr.setSampleRate(currentSampleRate);
    target->adsr.setParameters(adsrParams);
    target->adsr.noteOn();
}

void ClusterSynth::noteOff(int midiNote)
{
    for (auto& v : voices)
        if (v.active && v.midiNote == midiNote)
            v.adsr.noteOff();
}

float ClusterSynth::generateOsc(float phase, float shape)
{
    float p = phase - std::floor(phase);
    if (shape < 0.33f)
    {
        return std::sin(p * juce::MathConstants<float>::twoPi);
    }
    else if (shape < 0.66f)
    {
        return 2.0f * p - 1.0f; // saw
    }
    else
    {
        return p < 0.5f ? 1.0f : -1.0f; // square
    }
}

void ClusterSynth::renderBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    for (int sample = 0; sample < numSamples; ++sample)
    {
        float monoSample = 0.0f;
        for (auto& v : voices)
        {
            if (!v.active) continue;
            float env = v.adsr.getNextSample();
            if (!v.adsr.isActive()) { v.active = false; continue; }

            float voiceOut = 0.0f;
            for (int i = 0; i < numVoicesParam; ++i)
            {
                voiceOut += generateOsc(v.subPhases[i], shapeParam);
                v.subPhases[i] += v.subFreqs[i] / (float)currentSampleRate;
                if (v.subPhases[i] >= 1.0f) v.subPhases[i] -= 1.0f;
            }
            voiceOut /= (float)numVoicesParam;

            monoSample += voiceOut * env * v.velocity;
        }
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            buffer.addSample(ch, startSample + sample, monoSample * 0.25f);
    }
}

void ClusterSynth::setParameter(int index, float value)
{
    switch (index)
    {
        case 0: spread = value; break;
        case 1: numVoicesParam = 1 + (int)(value * 7.0f); break;
        case 2: detuneAmount = value; break;
        case 3: shapeParam = value; break;
    }
}

juce::String ClusterSynth::getParameterName(int index) const
{
    switch (index)
    {
        case 0: return "Spread";
        case 1: return "Voices";
        case 2: return "Detune";
        case 3: return "Shape";
        default: return "";
    }
}
