#include "SamplerEngine.h"

SamplerEngine::SamplerEngine()
{
    sampleBuffer.resize(maxRecordLength, 0.0f);
    for (auto& v : voices) v.active = false;
}

void SamplerEngine::prepare(double sampleRate, int spb)
{
    SynthEngine::prepare(sampleRate, spb);
    for (auto& v : voices)
    {
        v.adsr.setSampleRate(sampleRate);
        v.active = false;
    }
}

void SamplerEngine::startRecording()
{
    recording = true;
    recordPos = 0;
}

void SamplerEngine::stopRecording()
{
    recording = false;
    sampleLength = recordPos;
}

void SamplerEngine::recordSample(const float* data, int numSamples)
{
    if (!recording) return;
    for (int i = 0; i < numSamples && recordPos < maxRecordLength; ++i)
    {
        sampleBuffer[(size_t)recordPos++] = data[i];
    }
    if (recordPos >= maxRecordLength)
        stopRecording();
}

void SamplerEngine::noteOn(int midiNote, float velocity)
{
    if (sampleLength == 0) return; // Nothing recorded

    SamplerVoice* target = nullptr;
    for (auto& v : voices)
        if (!v.active) { target = &v; break; }
    if (!target) target = &voices[0];

    target->midiNote = midiNote;
    target->frequency = midiNoteToFreq(midiNote);
    target->velocity = velocity;
    target->active = true;
    target->playing = true;

    // Pitch shift based on note relative to base note
    target->playbackRate = std::pow(2.0f, (midiNote - baseNote) / 12.0f);
    target->playbackPos = startPos * (float)sampleLength;

    target->adsr.setSampleRate(currentSampleRate);
    target->adsr.setParameters(adsrParams);
    target->adsr.noteOn();
}

void SamplerEngine::noteOff(int midiNote)
{
    for (auto& v : voices)
        if (v.active && v.midiNote == midiNote)
            v.adsr.noteOff();
}

void SamplerEngine::renderBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    if (sampleLength == 0) return;

    float sampleEnd = endPos * (float)sampleLength;
    float sampleStart = startPos * (float)sampleLength;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float monoSample = 0.0f;
        for (auto& v : voices)
        {
            if (!v.active || !v.playing) continue;
            float env = v.adsr.getNextSample();
            if (!v.adsr.isActive()) { v.active = false; continue; }

            // Linear interpolation
            int pos0 = (int)v.playbackPos;
            int pos1 = pos0 + 1;
            float frac = v.playbackPos - (float)pos0;

            if (pos0 >= 0 && pos1 < sampleLength)
            {
                float s0 = sampleBuffer[(size_t)pos0];
                float s1 = sampleBuffer[(size_t)pos1];
                float interpolated = s0 + frac * (s1 - s0);
                monoSample += interpolated * env * v.velocity;
            }

            v.playbackPos += v.playbackRate;

            // Loop or stop
            if (v.playbackPos >= sampleEnd)
            {
                if (loopAmount > 0.01f)
                    v.playbackPos = sampleStart;
                else
                    v.active = false;
            }
        }
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            buffer.addSample(ch, startSample + sample, monoSample * 0.4f);
    }
}

void SamplerEngine::setParameter(int index, float value)
{
    switch (index)
    {
        case 0: startPos = value; break;
        case 1: endPos = juce::jmax(startPos + 0.01f, value); break;
        case 2: loopAmount = value; break;
        case 3: grainSize = value; break;
    }
}

juce::String SamplerEngine::getParameterName(int index) const
{
    switch (index)
    {
        case 0: return "Start";
        case 1: return "End";
        case 2: return "Loop";
        case 3: return "Grain";
        default: return "";
    }
}
