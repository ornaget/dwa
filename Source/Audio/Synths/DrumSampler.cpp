#include "DrumSampler.h"
#include <cmath>

DrumSampler::DrumSampler()
{
    for (auto& v : drumVoices) v.active = false;
}

void DrumSampler::prepare(double sampleRate, int spb)
{
    SynthEngine::prepare(sampleRate, spb);
}

void DrumSampler::noteOn(int midiNote, float velocity)
{
    // Map MIDI notes to drum types (like OP-1 keyboard layout)
    int drumType = 0;
    int noteInOctave = midiNote % 12;
    switch (noteInOctave)
    {
        case 0:  drumType = 0; break; // C - Kick
        case 2:  drumType = 1; break; // D - Snare
        case 4:  drumType = 2; break; // E - Hihat
        case 5:  drumType = 3; break; // F - Clap
        case 7:  drumType = 4; break; // G - Tom
        case 9:  drumType = 5; break; // A - Rim
        case 11: drumType = 6; break; // B - Cowbell
        default: drumType = 7; break; // Others - Cymbal
    }

    DrumVoice* target = nullptr;
    for (auto& v : drumVoices)
        if (!v.active) { target = &v; break; }
    if (!target)
    {
        // Steal voice of same type, or oldest
        for (auto& v : drumVoices)
            if (v.type == drumType) { target = &v; break; }
        if (!target) target = &drumVoices[0];
    }

    target->type = drumType;
    target->velocity = velocity;
    target->phase = 0.0f;
    target->samplePos = 0.0f;
    target->pitchEnv = 1.0f;
    target->ampEnv = 1.0f;
    target->frequency = midiNoteToFreq(midiNote);
    target->active = true;
}

void DrumSampler::noteOff(int /*midiNote*/)
{
    // Drums are one-shot, no note off needed
}

float DrumSampler::generateKick(DrumVoice& v)
{
    float baseFreq = 50.0f * (0.5f + tone);
    float pitchDecay = std::exp(-v.samplePos * (5.0f + snap * 20.0f));
    float freq = baseFreq + baseFreq * 4.0f * pitchDecay;
    float ampDecay = std::exp(-v.samplePos * (2.0f + (1.0f - decay) * 10.0f));
    v.ampEnv = ampDecay;

    v.phase += freq / (float)currentSampleRate;
    if (v.phase >= 1.0f) v.phase -= 1.0f;

    float sample = std::sin(v.phase * juce::MathConstants<float>::twoPi);

    // Click transient
    float click = 0.0f;
    if (v.samplePos < 0.005f)
        click = (1.0f - v.samplePos / 0.005f) * snap;

    return (sample + click) * ampDecay;
}

float DrumSampler::generateSnare(DrumVoice& v)
{
    float baseFreq = 180.0f * (0.5f + tone * 0.5f);
    float ampDecay = std::exp(-v.samplePos * (4.0f + (1.0f - decay) * 15.0f));
    v.ampEnv = ampDecay;

    v.phase += baseFreq / (float)currentSampleRate;
    if (v.phase >= 1.0f) v.phase -= 1.0f;

    float toneComponent = std::sin(v.phase * juce::MathConstants<float>::twoPi) * 0.6f;

    // Noise component using persistent RNG
    float noise = (rng.nextFloat() * 2.0f - 1.0f) * snap;

    // Snare wire rattle (filtered noise)
    float noiseDecay = std::exp(-v.samplePos * (6.0f + (1.0f - decay) * 10.0f));

    return (toneComponent + noise * noiseDecay) * ampDecay;
}

float DrumSampler::generateHihat(DrumVoice& v)
{
    float noise = rng.nextFloat() * 2.0f - 1.0f;

    // Multiple metallic frequencies
    float metal1 = std::sin(v.phase * juce::MathConstants<float>::twoPi * 2413.0f / 440.0f);
    float metal2 = std::sin(v.phase * juce::MathConstants<float>::twoPi * 3531.0f / 440.0f);

    v.phase += 440.0f / (float)currentSampleRate;
    if (v.phase >= 1.0f) v.phase -= 1.0f;

    float decayRate = 8.0f + (1.0f - decay) * 40.0f;
    float ampDecay = std::exp(-v.samplePos * decayRate);
    v.ampEnv = ampDecay;

    float brightness = tone;
    return (noise * 0.5f + (metal1 + metal2) * 0.25f * brightness) * ampDecay;
}

float DrumSampler::generateClap(DrumVoice& v)
{
    float noise = rng.nextFloat() * 2.0f - 1.0f;

    // Multiple micro-attacks (clap layering)
    float env = 0.0f;
    float pos = v.samplePos * (float)currentSampleRate;
    if (pos < 20.0f) env = 1.0f;
    else if (pos < 40.0f) env = 0.3f;
    else if (pos < 60.0f) env = 0.8f;
    else env = std::exp(-(v.samplePos - 60.0f / (float)currentSampleRate) * (5.0f + (1.0f - decay) * 20.0f));
    v.ampEnv = env;

    return noise * env * juce::jmax(tone, 0.01f);
}

float DrumSampler::generateTom(DrumVoice& v)
{
    float baseFreq = 100.0f * (0.5f + tone);
    float pitchDecay = std::exp(-v.samplePos * 3.0f);
    float freq = baseFreq + baseFreq * pitchDecay;
    float ampDecay = std::exp(-v.samplePos * (3.0f + (1.0f - decay) * 8.0f));
    v.ampEnv = ampDecay;

    v.phase += freq / (float)currentSampleRate;
    if (v.phase >= 1.0f) v.phase -= 1.0f;

    return std::sin(v.phase * juce::MathConstants<float>::twoPi) * ampDecay;
}

float DrumSampler::generateRim(DrumVoice& v)
{
    // Prevent division by zero when tone is 0
    float safeTone = juce::jmax(tone, 0.01f);
    float freq1 = 500.0f * safeTone;
    float freq2 = 800.0f * safeTone;

    v.phase += freq1 / (float)currentSampleRate;
    if (v.phase >= 1.0f) v.phase -= 1.0f;

    float osc = std::sin(v.phase * juce::MathConstants<float>::twoPi);
    float osc2 = std::sin(v.phase * juce::MathConstants<float>::twoPi * freq2 / freq1);
    float ampDecay = std::exp(-v.samplePos * (20.0f + (1.0f - decay) * 40.0f));
    v.ampEnv = ampDecay;

    return (osc * 0.5f + osc2 * 0.5f) * ampDecay * snap;
}

float DrumSampler::generateCowbell(DrumVoice& v)
{
    float freq1 = 545.0f * (0.8f + tone * 0.4f);
    float freq2 = 810.0f * (0.8f + tone * 0.4f);

    v.phase += freq1 / (float)currentSampleRate;
    if (v.phase >= 1.0f) v.phase -= 1.0f;

    float osc1 = (v.phase < 0.5f) ? 1.0f : -1.0f;
    float p2 = v.phase * freq2 / freq1;
    p2 -= std::floor(p2);
    float osc2 = (p2 < 0.5f) ? 1.0f : -1.0f;

    float ampDecay = std::exp(-v.samplePos * (3.0f + (1.0f - decay) * 10.0f));
    v.ampEnv = ampDecay;
    return (osc1 + osc2) * 0.3f * ampDecay;
}

float DrumSampler::generateCymbal(DrumVoice& v)
{
    float noise = rng.nextFloat() * 2.0f - 1.0f;

    // Rich metallic partials
    float metal = 0.0f;
    float freqs[] = { 2043.0f, 3141.0f, 4217.0f, 5318.0f, 6527.0f };
    for (int i = 0; i < 5; ++i)
    {
        float p = v.phase * freqs[i] / 440.0f;
        p -= std::floor(p);
        metal += std::sin(p * juce::MathConstants<float>::twoPi) * (1.0f / (float)(i + 1));
    }

    v.phase += 440.0f / (float)currentSampleRate;
    if (v.phase >= 1.0f) v.phase -= 1.0f;

    float ampDecay = std::exp(-v.samplePos * (1.0f + (1.0f - decay) * 5.0f));
    v.ampEnv = ampDecay;
    return (noise * 0.3f + metal * 0.7f * tone) * ampDecay;
}

void DrumSampler::renderBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    float sampleInc = 1.0f / (float)currentSampleRate;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float monoSample = 0.0f;
        for (auto& v : drumVoices)
        {
            if (!v.active) continue;

            float drumSample = 0.0f;
            switch (v.type)
            {
                case 0: drumSample = generateKick(v); break;
                case 1: drumSample = generateSnare(v); break;
                case 2: drumSample = generateHihat(v); break;
                case 3: drumSample = generateClap(v); break;
                case 4: drumSample = generateTom(v); break;
                case 5: drumSample = generateRim(v); break;
                case 6: drumSample = generateCowbell(v); break;
                case 7: drumSample = generateCymbal(v); break;
            }

            // Apply distortion
            if (distortion > 0.01f)
            {
                float drive = 1.0f + distortion * 10.0f;
                drumSample = std::tanh(drumSample * drive) / std::tanh(drive);
            }

            monoSample += drumSample * v.velocity;

            v.samplePos += sampleInc;
            // Deactivate voice when amplitude envelope has decayed
            if (v.ampEnv < 0.001f) v.active = false;
        }

        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            buffer.addSample(ch, startSample + sample, monoSample * 0.4f);
    }
}

void DrumSampler::setParameter(int index, float value)
{
    switch (index)
    {
        case 0: tone = value; break;
        case 1: decay = value; break;
        case 2: snap = value; break;
        case 3: distortion = value; break;
    }
}

juce::String DrumSampler::getParameterName(int index) const
{
    switch (index)
    {
        case 0: return "Tone";
        case 1: return "Decay";
        case 2: return "Snap";
        case 3: return "Drive";
        default: return "";
    }
}
