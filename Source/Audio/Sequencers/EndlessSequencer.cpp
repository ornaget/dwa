#include "EndlessSequencer.h"

EndlessSequencer::EndlessSequencer()
{
    for (auto& s : steps) s.active = false;
}

void EndlessSequencer::addNote(int step, int midiNote, float velocity)
{
    if (step >= 0 && step < numSteps)
    {
        steps[(size_t)step].midiNote = midiNote;
        steps[(size_t)step].velocity = velocity;
        steps[(size_t)step].active = true;
    }
}

void EndlessSequencer::clearStep(int step)
{
    if (step >= 0 && step < numSteps)
        steps[(size_t)step].active = false;
}

void EndlessSequencer::clearAll()
{
    for (auto& s : steps) s.active = false;
}

void EndlessSequencer::setNumSteps(int newSteps)
{
    numSteps = juce::jlimit(1, maxSteps, newSteps);
}

void EndlessSequencer::processBlock(juce::MidiBuffer& midiOut, int numSamples)
{
    if (!isPlaying) return;

    int samplesPerStep = getSamplesPerStep();

    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Handle note off
        if (activeNoteOff >= 0)
        {
            noteOffCountdown--;
            if (noteOffCountdown <= 0)
            {
                midiOut.addEvent(juce::MidiMessage::noteOff(1, activeNoteOff), sample);
                activeNoteOff = -1;
            }
        }

        sampleCounter++;
        if (sampleCounter >= samplesPerStep)
        {
            sampleCounter = 0;
            currentStep = (currentStep + 1) % numSteps;

            auto& step = steps[(size_t)currentStep];
            if (step.active)
            {
                // Note off for previous note if still sounding
                if (activeNoteOff >= 0)
                    midiOut.addEvent(juce::MidiMessage::noteOff(1, activeNoteOff), sample);

                int vel = (int)(step.velocity * 127.0f);
                midiOut.addEvent(juce::MidiMessage::noteOn(1, step.midiNote, (juce::uint8)vel), sample);
                activeNoteOff = step.midiNote;
                noteOffCountdown = (int)(samplesPerStep * step.duration);
            }
        }
    }
}
