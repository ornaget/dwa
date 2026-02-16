#include "SequencerScreen.h"
#include "../LookAndFeel/OP1LookAndFeel.h"

void SequencerScreen::setStepActive(int step, bool active)
{
    if (step >= 0 && step < 128)
        steps[(size_t)step].active = active;
}

void SequencerScreen::setStepNote(int step, int note)
{
    if (step >= 0 && step < 128)
        steps[(size_t)step].midiNote = note;
}

void SequencerScreen::paint(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    auto headerArea = bounds.removeFromTop(16);
    auto gridArea   = bounds.removeFromTop(bounds.getHeight() / 2);
    auto pianoArea  = bounds;

    // --- Header: sequencer name in sequencerColor ---
    g.setColour(OP1Colors::sequencerColor);
    g.setFont(OP1LookAndFeel::getDisplayFont(11.0f));
    g.drawText(seqName.toUpperCase(), headerArea.removeFromLeft(80),
               juce::Justification::centredLeft);

    // Playing indicator dot in vuGreen
    if (isPlaying)
    {
        g.setColour(OP1Colors::vuGreen);
        g.fillEllipse((float)headerArea.getX() + 2.0f,
                       (float)headerArea.getCentreY() - 3.0f, 6.0f, 6.0f);
    }

    drawStepGrid(g, gridArea);
    drawPianoRoll(g, pianoArea);
}

void SequencerScreen::drawStepGrid(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    bounds.reduce(2, 4);
    int visibleSteps = juce::jmin(numSteps, 32);
    float stepWidth  = (float)bounds.getWidth() / (float)visibleSteps;
    float stepHeight = (float)bounds.getHeight();

    for (int i = 0; i < visibleSteps; ++i)
    {
        auto stepBounds = juce::Rectangle<float>(
            (float)bounds.getX() + (float)i * stepWidth, (float)bounds.getY(),
            stepWidth - 1.0f, stepHeight);

        bool isActive  = steps[(size_t)i].active;
        bool isCurrent = (i == currentStep);

        if (isActive)
        {
            // Active steps: filled bars in sequencerColor, height based on MIDI note
            float noteNorm = (float)(steps[(size_t)i].midiNote - 36) / 60.0f;
            noteNorm = juce::jlimit(0.1f, 1.0f, noteNorm);

            float barHeight = stepHeight * (0.2f + noteNorm * 0.8f);
            auto barBounds = juce::Rectangle<float>(
                stepBounds.getX() + 1.0f,
                stepBounds.getBottom() - barHeight,
                stepBounds.getWidth() - 2.0f,
                barHeight);

            // Current step: full brightness. Other active steps: 50% alpha
            if (isCurrent)
                g.setColour(OP1Colors::sequencerColor);
            else
                g.setColour(OP1Colors::sequencerColor.withAlpha(0.50f));

            g.fillRoundedRectangle(barBounds, 2.0f);
        }
        else
        {
            // Inactive steps: textMuted at 12% alpha
            g.setColour(OP1Colors::textMuted.withAlpha(0.12f));
            g.fillRoundedRectangle(stepBounds.reduced(1.0f), 2.0f);
        }

        // Beat markers every 4 steps: 1px line at bottom in textDim at 40% alpha
        if (i % 4 == 0)
        {
            g.setColour(OP1Colors::textDim.withAlpha(0.40f));
            g.drawLine(stepBounds.getX(), stepBounds.getBottom(),
                       stepBounds.getRight(), stepBounds.getBottom(), 1.0f);
        }
    }
}

void SequencerScreen::drawPianoRoll(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    bounds.reduce(2, 2);

    int visibleSteps = juce::jmin(numSteps, 32);
    float stepWidth  = (float)bounds.getWidth() / (float)visibleSteps;

    // Note grid: 2 octaves
    int noteRange = 24;
    int baseNote  = 48;  // C3
    float noteHeight = (float)bounds.getHeight() / (float)noteRange;

    // --- Background grid: black key rows at textMuted 8% alpha ---
    for (int n = 0; n < noteRange; ++n)
    {
        int noteInOctave = (baseNote + n) % 12;
        bool isBlack = (noteInOctave == 1 || noteInOctave == 3 ||
                        noteInOctave == 6 || noteInOctave == 8 ||
                        noteInOctave == 10);

        if (isBlack)
        {
            float y = (float)bounds.getBottom() - (float)(n + 1) * noteHeight;
            g.setColour(OP1Colors::textMuted.withAlpha(0.08f));
            g.fillRect((float)bounds.getX(), y,
                       (float)bounds.getWidth(), noteHeight);
        }
    }

    // --- Note blocks: sequencerColor rounded rectangles ---
    for (int i = 0; i < visibleSteps; ++i)
    {
        if (!steps[(size_t)i].active)
            continue;

        int note    = steps[(size_t)i].midiNote;
        int relNote = note - baseNote;
        if (relNote < 0 || relNote >= noteRange)
            continue;

        float x = (float)bounds.getX() + (float)i * stepWidth;
        float y = (float)bounds.getBottom() - (float)(relNote + 1) * noteHeight;

        bool isCurrent = (i == currentStep);
        g.setColour(isCurrent ? OP1Colors::sequencerColor
                              : OP1Colors::sequencerColor.withAlpha(0.65f));
        g.fillRoundedRectangle(x + 0.5f, y + 0.5f,
                               stepWidth - 1.0f, noteHeight - 1.0f, 1.5f);
    }

    // --- Playhead: vertical line in textPrimary at 20% alpha ---
    if (currentStep < visibleSteps)
    {
        float px = (float)bounds.getX() + (float)currentStep * stepWidth;
        g.setColour(OP1Colors::textPrimary.withAlpha(0.20f));
        g.drawLine(px, (float)bounds.getY(), px, (float)bounds.getBottom(), 1.0f);
    }
}
