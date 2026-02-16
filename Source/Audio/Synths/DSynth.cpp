#include "DSynth.h"

DSynth::DSynth()
{
    for (auto& v : voices) v.active = false;
}

void DSynth::prepare(double sampleRate, int spb)
{
    SynthEngine::prepare(sampleRate, spb);
    for (auto& v : voices)
    {
        v.adsr.setSampleRate(sampleRate);
        v.active = false;
    }
}

void DSynth::noteOn(int midiNote, float velocity)
{
    DSynthVoice* target = nullptr;
    for (auto& v : voices)
        if (!v.active) { target = &v; break; }
    if (!target) target = &voices[0];

    target->midiNote = midiNote;
    target->frequency = midiNoteToFreq(midiNote);
    target->velocity = velocity;
    target->phase = 0.0f;
    target->phase2 = 0.0f;
    target->phase3 = 0.0f;
    target->accumulator = 0.0f;
    target->active = true;
    target->adsr.setSampleRate(currentSampleRate);
    target->adsr.setParameters(adsrParams);
    target->adsr.noteOn();
}

void DSynth::noteOff(int midiNote)
{
    for (auto& v : voices)
        if (v.active && v.midiNote == midiNote)
            v.adsr.noteOff();
}

float DSynth::runAlgorithm(DSynthVoice& v, float freq)
{
    float inc = freq / (float)currentSampleRate;
    float twoPi = juce::MathConstants<float>::twoPi;

    switch (algorithm)
    {
        case 0: // Additive harmonics
        {
            float out = 0.0f;
            int harmonics = 1 + (int)(param1 * 15.0f);
            for (int h = 1; h <= harmonics; ++h)
            {
                float amp = 1.0f / (float)h * (1.0f - param2 * 0.5f);
                out += std::sin(v.phase * twoPi * (float)h) * amp;
            }
            return out * 0.5f;
        }
        case 1: // Phase distortion
        {
            float p = v.phase;
            float d = param1;
            if (p < d)
                p = p / d * 0.5f;
            else
                p = 0.5f + (p - d) / (1.0f - d) * 0.5f;
            return std::sin(p * twoPi) * (1.0f - param2 * 0.5f);
        }
        case 2: // Ring mod
        {
            float osc1 = std::sin(v.phase * twoPi);
            float modFreq = freq * (1.0f + param1 * 7.0f);
            v.phase2 += modFreq / (float)currentSampleRate;
            if (v.phase2 >= 1.0f) v.phase2 -= 1.0f;
            float osc2 = std::sin(v.phase2 * twoPi);
            return (osc1 * osc2 * param2 + osc1 * (1.0f - param2));
        }
        case 3: // Noise + tone
        {
            juce::Random rng;
            float tone = std::sin(v.phase * twoPi);
            float noise = rng.nextFloat() * 2.0f - 1.0f;
            return tone * (1.0f - param1) + noise * param1 * param2;
        }
        case 4: // Wavetable-ish crossfade
        {
            float sine = std::sin(v.phase * twoPi);
            float saw = 2.0f * v.phase - 1.0f;
            float square = v.phase < 0.5f ? 1.0f : -1.0f;
            float tri = 4.0f * std::abs(v.phase - 0.5f) - 1.0f;

            float mix1 = sine * (1.0f - param1) + saw * param1;
            float mix2 = square * (1.0f - param1) + tri * param1;
            return mix1 * (1.0f - param2) + mix2 * param2;
        }
        case 5: // Bit crush
        {
            float sine = std::sin(v.phase * twoPi);
            int bits = 2 + (int)((1.0f - param1) * 14.0f);
            float levels = (float)(1 << bits);
            float crushed = std::round(sine * levels) / levels;
            // Sample rate reduction
            float holdRate = 1.0f + param2 * 50.0f;
            v.accumulator += inc * holdRate;
            if (v.accumulator >= 1.0f)
            {
                v.accumulator -= 1.0f;
                return crushed;
            }
            return crushed;
        }
        case 6: // Formant
        {
            float formant1 = param1 * 2000.0f + 200.0f;
            float formant2 = param2 * 3000.0f + 500.0f;
            float carrier = std::sin(v.phase * twoPi);
            float mod1 = std::sin(v.phase * twoPi * formant1 / freq);
            float mod2 = std::sin(v.phase * twoPi * formant2 / freq);
            return carrier * 0.5f + mod1 * 0.3f + mod2 * 0.2f;
        }
        case 7: // Supersaw
        {
            float out = 0.0f;
            int saws = 3 + (int)(param1 * 4.0f);
            for (int i = 0; i < saws; ++i)
            {
                float detune = (float)(i - saws / 2) * param2 * 0.01f;
                float p = v.phase * (1.0f + detune);
                p = p - std::floor(p);
                out += 2.0f * p - 1.0f;
            }
            return out / (float)saws;
        }
        default:
            return std::sin(v.phase * twoPi);
    }
}

void DSynth::renderBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    for (int sample = 0; sample < numSamples; ++sample)
    {
        float monoSample = 0.0f;
        for (auto& v : voices)
        {
            if (!v.active) continue;
            float env = v.adsr.getNextSample();
            if (!v.adsr.isActive()) { v.active = false; continue; }

            float out = runAlgorithm(v, v.frequency);
            out *= param3; // output level per algorithm

            monoSample += out * env * v.velocity;

            float inc = v.frequency / (float)currentSampleRate;
            v.phase += inc;
            if (v.phase >= 1.0f) v.phase -= 1.0f;
        }
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            buffer.addSample(ch, startSample + sample, monoSample * 0.25f);
    }
}

void DSynth::setParameter(int index, float value)
{
    switch (index)
    {
        case 0: algorithm = (int)(value * 7.0f); break;
        case 1: param1 = value; break;
        case 2: param2 = value; break;
        case 3: param3 = value; break;
    }
}

juce::String DSynth::getParameterName(int index) const
{
    switch (index)
    {
        case 0: return "Algorithm";
        case 1: return "Param 1";
        case 2: return "Param 2";
        case 3: return "Level";
        default: return "";
    }
}
