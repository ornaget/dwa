#include "SketchSequencer.h"

SketchSequencer::SketchSequencer()
{
    for (auto& s : steps) s.active = false;
}

void SketchSequencer::setNoteAtStep(int step, int midiNote)
{
    if (step >= 0 && step < numSteps)
    {
        steps[(size_t)step].midiNote = midiNote;
        steps[(size_t)step].velocity = 0.8f;
        steps[(size_t)step].active = true;
    }
}

void SketchSequencer::setVelocityAtStep(int step, float velocity)
{
    if (step >= 0 && step < numSteps)
        steps[(size_t)step].velocity = velocity;
}

void SketchSequencer::clearStep(int step)
{
    if (step >= 0 && step < numSteps)
        steps[(size_t)step].active = false;
}

void SketchSequencer::clearAll()
{
    for (auto& s : steps) s.active = false;
}

void SketchSequencer::setNumSteps(int newSteps)
{
    numSteps = juce::jlimit(1, maxSteps, newSteps);
}

int SketchSequencer::getNoteAtStep(int step) const
{
    if (step >= 0 && step < numSteps)
        return steps[(size_t)step].midiNote;
    return 60;
}

float SketchSequencer::getVelocityAtStep(int step) const
{
    if (step >= 0 && step < numSteps)
        return steps[(size_t)step].velocity;
    return 0.0f;
}

bool SketchSequencer::isStepActive(int step) const
{
    if (step >= 0 && step < numSteps)
        return steps[(size_t)step].active;
    return false;
}

void SketchSequencer::processBlock(juce::MidiBuffer& midiOut, int numSamples)
{
    if (!isPlaying) return;

    int samplesPerStep = getSamplesPerStep();

    for (int sample = 0; sample < numSamples; ++sample)
    {
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
                if (activeNoteOff >= 0)
                    midiOut.addEvent(juce::MidiMessage::noteOff(1, activeNoteOff), sample);

                int vel = (int)(step.velocity * 127.0f);
                midiOut.addEvent(juce::MidiMessage::noteOn(1, step.midiNote, (juce::uint8)vel), sample);
                activeNoteOff = step.midiNote;
                noteOffCountdown = (int)(samplesPerStep * 0.8f);
            }
        }
    }
}
