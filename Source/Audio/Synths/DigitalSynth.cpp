#include "DigitalSynth.h"

DigitalSynth::DigitalSynth()
{
    for (auto& v : voices) v.active = false;
}

void DigitalSynth::prepare(double sampleRate, int spb)
{
    SynthEngine::prepare(sampleRate, spb);
    for (auto& v : voices)
    {
        v.adsr.setSampleRate(sampleRate);
        v.active = false;
    }
}

void DigitalSynth::noteOn(int midiNote, float velocity)
{
    DigiVoice* target = nullptr;
    for (auto& v : voices)
        if (!v.active) { target = &v; break; }
    if (!target) target = &voices[0];

    target->midiNote = midiNote;
    target->frequency = midiNoteToFreq(midiNote);
    target->velocity = velocity;
    target->phase = 0.0f;
    target->phase2 = phaseOffset;
    target->active = true;
    target->adsr.setSampleRate(currentSampleRate);
    target->adsr.setParameters(adsrParams);
    target->adsr.noteOn();
}

void DigitalSynth::noteOff(int midiNote)
{
    for (auto& v : voices)
        if (v.active && v.midiNote == midiNote)
            v.adsr.noteOff();
}

float DigitalSynth::generateWaveform(float phase, float wf)
{
    // Morph: sine -> saw -> square -> noise-ish digital
    float p = phase - std::floor(phase);

    if (wf < 0.25f)
    {
        float t = wf / 0.25f;
        float sine = std::sin(p * juce::MathConstants<float>::twoPi);
        float saw = 2.0f * p - 1.0f;
        return sine * (1.0f - t) + saw * t;
    }
    else if (wf < 0.5f)
    {
        float t = (wf - 0.25f) / 0.25f;
        float saw = 2.0f * p - 1.0f;
        float square = p < 0.5f ? 1.0f : -1.0f;
        return saw * (1.0f - t) + square * t;
    }
    else if (wf < 0.75f)
    {
        float t = (wf - 0.5f) / 0.25f;
        float square = p < 0.5f ? 1.0f : -1.0f;
        float tri = 4.0f * std::abs(p - 0.5f) - 1.0f;
        return square * (1.0f - t) + tri * t;
    }
    else
    {
        float t = (wf - 0.75f) / 0.25f;
        float tri = 4.0f * std::abs(p - 0.5f) - 1.0f;
        // Digital harsh wave
        float digital = std::sin(p * juce::MathConstants<float>::twoPi * 3.0f) *
                         std::sin(p * juce::MathConstants<float>::twoPi * 7.0f);
        return tri * (1.0f - t) + digital * t;
    }
}

float DigitalSynth::applyFold(float sample, float foldAmount)
{
    if (foldAmount < 0.01f) return sample;
    float gain = 1.0f + foldAmount * 8.0f;
    float s = sample * gain;
    // Wave folding
    while (s > 1.0f || s < -1.0f)
    {
        if (s > 1.0f) s = 2.0f - s;
        if (s < -1.0f) s = -2.0f - s;
    }
    return s;
}

void DigitalSynth::renderBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    for (int sample = 0; sample < numSamples; ++sample)
    {
        float monoSample = 0.0f;
        for (auto& v : voices)
        {
            if (!v.active) continue;
            float env = v.adsr.getNextSample();
            if (!v.adsr.isActive()) { v.active = false; continue; }

            float inc1 = v.frequency / (float)currentSampleRate;
            float detuneFreq = v.frequency * (1.0f + detune * 0.02f);
            float inc2 = detuneFreq / (float)currentSampleRate;

            float osc1 = generateWaveform(v.phase, waveform);
            float osc2 = generateWaveform(v.phase2, waveform);

            float mixed = (osc1 + osc2 * 0.7f) * 0.6f;
            mixed = applyFold(mixed, fold);

            monoSample += mixed * env * v.velocity;

            v.phase += inc1;
            v.phase2 += inc2;
            if (v.phase >= 1.0f) v.phase -= 1.0f;
            if (v.phase2 >= 1.0f) v.phase2 -= 1.0f;
        }
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            buffer.addSample(ch, startSample + sample, monoSample * 0.3f);
    }
}

void DigitalSynth::setParameter(int index, float value)
{
    switch (index)
    {
        case 0: waveform = value; break;
        case 1: detune = value; break;
        case 2: phaseOffset = value; break;
        case 3: fold = value; break;
    }
}

juce::String DigitalSynth::getParameterName(int index) const
{
    switch (index)
    {
        case 0: return "Waveform";
        case 1: return "Detune";
        case 2: return "Phase";
        case 3: return "Fold";
        default: return "";
    }
}
