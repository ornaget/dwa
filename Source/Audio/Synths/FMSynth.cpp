#include "FMSynth.h"

FMSynth::FMSynth()
{
    for (auto& v : voices)
        v.active = false;
}

void FMSynth::prepare(double sampleRate, int spb)
{
    SynthEngine::prepare(sampleRate, spb);
    for (auto& v : voices)
    {
        v.adsr.setSampleRate(sampleRate);
        v.active = false;
    }
}

void FMSynth::noteOn(int midiNote, float velocity)
{
    // Find free voice or steal oldest
    FMVoice* target = nullptr;
    for (auto& v : voices)
    {
        if (!v.active) { target = &v; break; }
    }
    if (!target) target = &voices[0]; // voice stealing

    target->midiNote = midiNote;
    target->frequency = midiNoteToFreq(midiNote);
    target->velocity = velocity;
    target->phase = 0.0f;
    target->modPhase = 0.0f;
    target->active = true;
    target->adsr.setSampleRate(currentSampleRate);
    target->adsr.setParameters(adsrParams);
    target->adsr.noteOn();
}

void FMSynth::noteOff(int midiNote)
{
    for (auto& v : voices)
    {
        if (v.active && v.midiNote == midiNote)
        {
            v.adsr.noteOff();
        }
    }
}

float FMSynth::generateCarrier(float phase, float shapeAmount)
{
    // Morph between sine, triangle, saw based on shape
    float sine = std::sin(phase * juce::MathConstants<float>::twoPi);
    if (shapeAmount < 0.33f)
    {
        return sine;
    }
    else if (shapeAmount < 0.66f)
    {
        float t = (shapeAmount - 0.33f) / 0.33f;
        float tri = 2.0f * std::abs(2.0f * (phase - std::floor(phase + 0.5f))) - 1.0f;
        return sine * (1.0f - t) + tri * t;
    }
    else
    {
        float t = (shapeAmount - 0.66f) / 0.34f;
        float tri = 2.0f * std::abs(2.0f * (phase - std::floor(phase + 0.5f))) - 1.0f;
        float saw = 2.0f * (phase - std::floor(phase + 0.5f));
        return tri * (1.0f - t) + saw * t;
    }
}

void FMSynth::renderBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    for (int sample = 0; sample < numSamples; ++sample)
    {
        float monoSample = 0.0f;
        for (auto& v : voices)
        {
            if (!v.active) continue;

            float envValue = v.adsr.getNextSample();
            if (!v.adsr.isActive())
            {
                v.active = false;
                continue;
            }

            float modFreq = v.frequency * ratio;
            float phaseInc = v.frequency / (float)currentSampleRate;
            float modPhaseInc = modFreq / (float)currentSampleRate;

            // Modulator with per-voice feedback
            float modSignal = std::sin(v.modPhase * juce::MathConstants<float>::twoPi
                                       + feedback * v.lastModOutput * juce::MathConstants<float>::pi);
            v.lastModOutput = modSignal;

            // Apply FM modulation to carrier phase
            float modulatedPhase = v.phase + depth * modSignal * 4.0f;
            float carrierSample = generateCarrier(modulatedPhase, shape);

            monoSample += carrierSample * envValue * v.velocity;

            v.phase += phaseInc;
            v.modPhase += modPhaseInc;
            if (v.phase >= 1.0f) v.phase -= 1.0f;
            if (v.modPhase >= 1.0f) v.modPhase -= 1.0f;
        }

        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            buffer.addSample(ch, startSample + sample, monoSample * 0.3f);
    }
}

void FMSynth::setParameter(int index, float value)
{
    switch (index)
    {
        case 0: ratio = 0.5f + value * 15.5f; break;
        case 1: depth = value; break;
        case 2: feedback = value; break;
        case 3: shape = value; break;
    }
}

juce::String FMSynth::getParameterName(int index) const
{
    switch (index)
    {
        case 0: return "Ratio";
        case 1: return "Depth";
        case 2: return "Feedback";
        case 3: return "Shape";
        default: return "";
    }
}
