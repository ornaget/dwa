#include "TapeRecorder.h"
#include <cmath>

TapeRecorder::TapeRecorder()
{
    for (auto& track : tracks)
        track.resize((size_t)maxTapeLength, 0.0f);
}

void TapeRecorder::prepare(double sr, int spb)
{
    sampleRate = sr;
    blockSize = spb;
}

void TapeRecorder::play()
{
    playing.store(true);
    recording.store(false);
}

void TapeRecorder::stop()
{
    playing.store(false);
    recording.store(false);
}

void TapeRecorder::record()
{
    playing.store(true);
    recording.store(true);
}

void TapeRecorder::rewind()
{
    float ph = playHead.load();
    ph -= (float)sampleRate * 2.0f; // rewind 2 seconds
    if (ph < 0.0f) ph = 0.0f;
    playHead.store(ph);
}

void TapeRecorder::fastForward()
{
    float ph = playHead.load();
    ph += (float)sampleRate * 2.0f;
    if (ph >= (float)maxTapeLength)
        ph = (float)(maxTapeLength - 1);
    playHead.store(ph);
}

void TapeRecorder::setTrackVolume(int track, float volume)
{
    if (track >= 0 && track < numTracks)
        trackVolumes[(size_t)track] = volume;
}

float TapeRecorder::getTrackVolume(int track) const
{
    if (track >= 0 && track < numTracks)
        return trackVolumes[(size_t)track];
    return 0.0f;
}

float TapeRecorder::getPlaybackPosition() const
{
    return playHead.load() / (float)maxTapeLength;
}

float TapeRecorder::getTapeLength() const
{
    return (float)maxTapeLength / (float)sampleRate;
}

float TapeRecorder::saturate(float sample)
{
    // Tape saturation: soft clipping with slight warmth
    return std::tanh(sample * 1.2f) * 0.85f;
}

void TapeRecorder::process(juce::AudioBuffer<float>& inputBuffer, juce::AudioBuffer<float>& outputBuffer)
{
    if (!playing.load())
    {
        outputBuffer.clear();
        return;
    }

    bool isRecording = recording.load();
    float currentPlayHead = playHead.load();
    int numSamples = outputBuffer.getNumSamples();
    float loopStart = loopInPos * (float)maxTapeLength;
    float loopEnd = loopOutPos * (float)maxTapeLength;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        int pos = (int)currentPlayHead;
        if (pos < 0) pos = 0;
        if (pos >= maxTapeLength) pos = maxTapeLength - 1;

        // Record input to active track
        if (isRecording && inputBuffer.getNumChannels() > 0)
        {
            float inputSample = 0.0f;
            for (int ch = 0; ch < inputBuffer.getNumChannels(); ++ch)
                inputSample += inputBuffer.getSample(ch, sample);
            inputSample /= (float)inputBuffer.getNumChannels();

            // Overdub: mix with existing content (clamp to prevent unbounded growth)
            float accumulated = tracks[(size_t)activeTrack][(size_t)pos] + saturate(inputSample);
            tracks[(size_t)activeTrack][(size_t)pos] = juce::jlimit(-2.0f, 2.0f, accumulated);
        }

        // Mix all tracks for output
        float mixL = 0.0f;
        float mixR = 0.0f;
        for (int t = 0; t < numTracks; ++t)
        {
            float trackSample = tracks[(size_t)t][(size_t)pos] * trackVolumes[(size_t)t];

            // Simple stereo panning: tracks spread across stereo field
            float pan = (float)t / (float)(numTracks - 1); // 0 to 1
            mixL += trackSample * (1.0f - pan * 0.5f);
            mixR += trackSample * (0.5f + pan * 0.5f);
        }

        // Apply tape saturation to output
        mixL = saturate(mixL);
        mixR = saturate(mixR);

        if (outputBuffer.getNumChannels() >= 1)
            outputBuffer.setSample(0, sample, mixL);
        if (outputBuffer.getNumChannels() >= 2)
            outputBuffer.setSample(1, sample, mixR);

        // Advance playhead
        float increment = tapeSpeed * (reverse ? -1.0f : 1.0f);
        currentPlayHead += increment;

        // Loop handling
        if (loopEnabled)
        {
            if (!reverse && currentPlayHead >= loopEnd)
                currentPlayHead = loopStart;
            else if (reverse && currentPlayHead <= loopStart)
                currentPlayHead = loopEnd;
        }
        else
        {
            if (currentPlayHead >= (float)maxTapeLength)
            {
                currentPlayHead = (float)(maxTapeLength - 1);
                playing.store(false);
            }
            else if (currentPlayHead < 0.0f)
            {
                currentPlayHead = 0.0f;
                playing.store(false);
            }
        }
    }

    playHead.store(currentPlayHead);
}
