#include "PatternSequencer.h"

PatternSequencer::PatternSequencer()
{
    for (auto& p : patterns)
        for (auto& s : p.steps)
            s.active = false;
}

void PatternSequencer::toggleStep(int step, int midiNote, float velocity)
{
    if (step >= 0 && step < stepsPerPattern)
    {
        auto& s = patterns[(size_t)currentPattern].steps[(size_t)step];
        if (s.active && s.midiNote == midiNote)
            s.active = false;
        else
        {
            s.midiNote = midiNote;
            s.velocity = velocity;
            s.duration = 0.5f;
            s.active = true;
        }
    }
}

void PatternSequencer::setPattern(int patternIndex)
{
    currentPattern = juce::jlimit(0, numPatterns - 1, patternIndex);
}

bool PatternSequencer::isStepActive(int pattern, int step) const
{
    if (pattern >= 0 && pattern < numPatterns && step >= 0 && step < stepsPerPattern)
        return patterns[(size_t)pattern].steps[(size_t)step].active;
    return false;
}

const SequencerBase::NoteEvent& PatternSequencer::getStep(int pattern, int step) const
{
    return patterns[(size_t)pattern].steps[(size_t)step];
}

void PatternSequencer::processBlock(juce::MidiBuffer& midiOut, int numSamples)
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
            currentStep = (currentStep + 1) % stepsPerPattern;

            auto& step = patterns[(size_t)currentPattern].steps[(size_t)currentStep];
            if (step.active)
            {
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
