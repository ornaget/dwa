#include "PulseSynth.h"

PulseSynth::PulseSynth()
{
    for (auto& v : voices) v.active = false;
}

void PulseSynth::prepare(double sampleRate, int spb)
{
    SynthEngine::prepare(sampleRate, spb);
    for (auto& v : voices)
    {
        v.adsr.setSampleRate(sampleRate);
        v.active = false;
    }
}

void PulseSynth::noteOn(int midiNote, float velocity)
{
    PulseVoice* target = nullptr;
    for (auto& v : voices)
        if (!v.active) { target = &v; break; }
    if (!target) target = &voices[0];

    target->midiNote = midiNote;
    float detuneSemitones = fineTune;
    target->frequency = midiNoteToFreq(midiNote) * std::pow(2.0f, detuneSemitones / 12.0f);
    target->velocity = velocity;
    target->phase = 0.0f;
    target->syncPhase = 0.0f;
    target->active = true;
    target->adsr.setSampleRate(currentSampleRate);
    target->adsr.setParameters(adsrParams);
    target->adsr.noteOn();
}

void PulseSynth::noteOff(int midiNote)
{
    for (auto& v : voices)
        if (v.active && v.midiNote == midiNote)
            v.adsr.noteOff();
}

void PulseSynth::renderBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    for (int sample = 0; sample < numSamples; ++sample)
    {
        float monoSample = 0.0f;
        for (auto& v : voices)
        {
            if (!v.active) continue;
            float env = v.adsr.getNextSample();
            if (!v.adsr.isActive()) { v.active = false; continue; }

            float inc = v.frequency / (float)currentSampleRate;

            // Basic pulse wave
            float pw = pulseWidth;

            // Always advance syncPhase as a shape LFO (independent of sync)
            float shapeLfoRate = 2.0f; // Hz
            v.syncPhase += shapeLfoRate / (float)currentSampleRate;
            if (v.syncPhase >= 1.0f) v.syncPhase -= 1.0f;

            // Shape modulation: animate pulse width with LFO-like effect
            if (shapeAmount > 0.01f)
            {
                pw += shapeAmount * 0.3f * std::sin(v.syncPhase * juce::MathConstants<float>::twoPi);
                pw = juce::jlimit(0.05f, 0.95f, pw);
            }

            // Band-limited pulse wave with soft edge
            float softness = 0.02f;
            float edge1 = juce::jlimit(-1.0f, 1.0f, (v.phase - pw) / softness);
            float output = 1.0f - 2.0f * (0.5f + 0.5f * edge1);

            // Oscillator sync: blend with sync oscillator
            if (syncAmount > 0.01f)
            {
                float syncFreq = v.frequency * (1.0f + syncAmount * 3.0f);
                float syncPhase = std::fmod(v.phase * syncFreq / v.frequency, 1.0f);
                float syncEdge = juce::jlimit(-1.0f, 1.0f, (syncPhase - pw) / softness);
                float syncPulse = 1.0f - 2.0f * (0.5f + 0.5f * syncEdge);
                output = output * (1.0f - syncAmount) + syncPulse * syncAmount;
            }

            monoSample += output * env * v.velocity;

            v.phase += inc;
            if (v.phase >= 1.0f) v.phase -= 1.0f;
        }
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            buffer.addSample(ch, startSample + sample, monoSample * 0.25f);
    }
}

void PulseSynth::setParameter(int index, float value)
{
    switch (index)
    {
        case 0: pulseWidth = 0.01f + value * 0.98f; break;
        case 1: fineTune = (value - 0.5f) * 2.0f; break;
        case 2: shapeAmount = value; break;
        case 3: syncAmount = value; break;
    }
}

juce::String PulseSynth::getParameterName(int index) const
{
    switch (index)
    {
        case 0: return "Width";
        case 1: return "Fine";
        case 2: return "Shape";
        case 3: return "Sync";
        default: return "";
    }
}
