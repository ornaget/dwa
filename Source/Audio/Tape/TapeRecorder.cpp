#include "TapeRecorder.h"

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
    playing = true;
    recording = false;
}

void TapeRecorder::stop()
{
    playing = false;
    recording = false;
}

void TapeRecorder::record()
{
    playing = true;
    recording = true;
}

void TapeRecorder::rewind()
{
    playHead -= (float)sampleRate * 2.0f; // rewind 2 seconds
    if (playHead < 0.0f) playHead = 0.0f;
}

void TapeRecorder::fastForward()
{
    playHead += (float)sampleRate * 2.0f;
    if (playHead >= (float)maxTapeLength)
        playHead = (float)(maxTapeLength - 1);
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
    return playHead / (float)maxTapeLength;
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
    if (!playing)
    {
        outputBuffer.clear();
        return;
    }

    int numSamples = outputBuffer.getNumSamples();
    float loopStart = loopInPos * (float)maxTapeLength;
    float loopEnd = loopOutPos * (float)maxTapeLength;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        int pos = (int)playHead;
        if (pos < 0) pos = 0;
        if (pos >= maxTapeLength) pos = maxTapeLength - 1;

        // Record input to active track
        if (recording && inputBuffer.getNumChannels() > 0)
        {
            float inputSample = 0.0f;
            for (int ch = 0; ch < inputBuffer.getNumChannels(); ++ch)
                inputSample += inputBuffer.getSample(ch, sample);
            inputSample /= (float)inputBuffer.getNumChannels();

            // Overdub: mix with existing content
            tracks[(size_t)activeTrack][(size_t)pos] += saturate(inputSample);
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
        playHead += increment;

        // Loop handling
        if (loopEnabled)
        {
            if (!reverse && playHead >= loopEnd)
                playHead = loopStart;
            else if (reverse && playHead <= loopStart)
                playHead = loopEnd;
        }
        else
        {
            if (playHead >= (float)maxTapeLength)
            {
                playHead = (float)(maxTapeLength - 1);
                playing = false;
            }
            else if (playHead < 0.0f)
            {
                playHead = 0.0f;
                playing = false;
            }
        }
    }
}
