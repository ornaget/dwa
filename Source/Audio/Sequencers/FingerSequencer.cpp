#include "FingerSequencer.h"

FingerSequencer::FingerSequencer()
{
    recordedEvents.reserve(maxEvents);
}

void FingerSequencer::startRecording()
{
    isRecording = true;
    recordedEvents.clear();
    recordStartSample = 0;
    loopLengthSamples = getSamplesPerStep() * loopLengthSteps;
}

void FingerSequencer::stopRecording()
{
    isRecording = false;
    if (recordStartSample > 0)
        loopLengthSamples = recordStartSample;
}

void FingerSequencer::clearRecording()
{
    recordedEvents.clear();
    isRecording = false;
    playbackPosition = 0;
}

void FingerSequencer::recordNoteOn(int midiNote, float velocity)
{
    if (!isRecording || recordedEvents.size() >= maxEvents) return;
    recordedEvents.push_back({ midiNote, velocity, recordStartSample, true });
}

void FingerSequencer::recordNoteOff(int midiNote)
{
    if (!isRecording || recordedEvents.size() >= maxEvents) return;
    recordedEvents.push_back({ midiNote, 0.0f, recordStartSample, false });
}

void FingerSequencer::processBlock(juce::MidiBuffer& midiOut, int numSamples)
{
    if (!isPlaying && !isRecording) return;

    if (isRecording)
    {
        recordStartSample += numSamples;
        return;
    }

    if (recordedEvents.empty() || loopLengthSamples <= 0) return;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        for (const auto& event : recordedEvents)
        {
            if (event.samplePosition == playbackPosition)
            {
                if (event.isNoteOn)
                {
                    int vel = (int)(event.velocity * 127.0f);
                    midiOut.addEvent(juce::MidiMessage::noteOn(1, event.midiNote, (juce::uint8)vel), sample);
                }
                else
                {
                    midiOut.addEvent(juce::MidiMessage::noteOff(1, event.midiNote), sample);
                }
            }
        }

        playbackPosition++;
        if (playbackPosition >= loopLengthSamples)
        {
            playbackPosition = 0;
            currentStep = 0;
        }

        sampleCounter++;
        if (sampleCounter >= getSamplesPerStep())
        {
            sampleCounter = 0;
            currentStep++;
        }
    }
}
