#include "SequencerScreen.h"
#include "../LookAndFeel/OP1LookAndFeel.h"

void SequencerScreen::setStepActive(int step, bool active)
{
    if (step >= 0 && step < 128) steps[(size_t)step].active = active;
}

void SequencerScreen::setStepNote(int step, int note)
{
    if (step >= 0 && step < 128) steps[(size_t)step].midiNote = note;
}

void SequencerScreen::paint(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    auto headerArea = bounds.removeFromTop(14);
    auto gridArea = bounds.removeFromTop(bounds.getHeight() / 2);
    auto pianoArea = bounds;

    // Header
    g.setColour(OP1Colors::sequencerColor);
    g.setFont(OP1LookAndFeel::getOP1Font(11.0f));
    g.drawText(seqName.toUpperCase(), headerArea.removeFromLeft(80), juce::Justification::centredLeft);

    if (isPlaying)
    {
        g.setColour(OP1Colors::vuGreen);
        g.fillEllipse((float)headerArea.getX(), (float)headerArea.getCentreY() - 3, 6, 6);
    }

    drawStepGrid(g, gridArea);
    drawPianoRoll(g, pianoArea);
}

void SequencerScreen::drawStepGrid(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    bounds.reduce(2, 4);
    int visibleSteps = juce::jmin(numSteps, 32);
    float stepWidth = (float)bounds.getWidth() / (float)visibleSteps;
    float stepHeight = (float)bounds.getHeight();

    for (int i = 0; i < visibleSteps; ++i)
    {
        auto stepBounds = juce::Rectangle<float>(
            (float)bounds.getX() + (float)i * stepWidth, (float)bounds.getY(),
            stepWidth - 1.0f, stepHeight);

        bool isActive = steps[(size_t)i].active;
        bool isCurrent = (i == currentStep);

        // Step background
        if (isCurrent)
        {
            g.setColour(OP1Colors::sequencerColor.withAlpha(0.4f));
            g.fillRoundedRectangle(stepBounds, 2.0f);
        }

        if (isActive)
        {
            // Note height based on MIDI note
            float noteNorm = (float)(steps[(size_t)i].midiNote - 36) / 60.0f;
            noteNorm = juce::jlimit(0.0f, 1.0f, noteNorm);

            float barHeight = stepHeight * (0.2f + noteNorm * 0.8f);
            auto barBounds = juce::Rectangle<float>(
                stepBounds.getX() + 1.0f,
                stepBounds.getBottom() - barHeight,
                stepBounds.getWidth() - 2.0f,
                barHeight);

            juce::Colour stepColor = isCurrent ?
                OP1Colors::sequencerColor : OP1Colors::sequencerColor.withAlpha(0.6f);
            g.setColour(stepColor);
            g.fillRoundedRectangle(barBounds, 1.0f);
        }
        else
        {
            // Empty step indicator
            g.setColour(OP1Colors::textDim.withAlpha(0.2f));
            g.fillRoundedRectangle(stepBounds.reduced(1.0f), 1.0f);
        }

        // Beat markers (every 4th step)
        if (i % 4 == 0)
        {
            g.setColour(OP1Colors::textDim.withAlpha(0.5f));
            g.fillRect(stepBounds.getX(), stepBounds.getBottom() - 2.0f, stepBounds.getWidth(), 2.0f);
        }
    }
}

void SequencerScreen::drawPianoRoll(juce::Graphics& g, juce::Rectangle<int> bounds)
{
    bounds.reduce(2, 2);

    // Simplified piano roll view showing note positions
    int visibleSteps = juce::jmin(numSteps, 32);
    float stepWidth = (float)bounds.getWidth() / (float)visibleSteps;

    // Background grid
    int noteRange = 24; // 2 octaves
    int baseNote = 48; // C3
    float noteHeight = (float)bounds.getHeight() / (float)noteRange;

    // Draw note grid lines
    for (int n = 0; n < noteRange; ++n)
    {
        int note = baseNote + n;
        bool isBlack = false;
        int noteInOctave = note % 12;
        if (noteInOctave == 1 || noteInOctave == 3 || noteInOctave == 6 ||
            noteInOctave == 8 || noteInOctave == 10)
            isBlack = true;

        float y = bounds.getBottom() - (float)(n + 1) * noteHeight;
        g.setColour(isBlack ? OP1Colors::textDim.withAlpha(0.1f) : OP1Colors::textDim.withAlpha(0.05f));
        g.fillRect((float)bounds.getX(), y, (float)bounds.getWidth(), noteHeight);
    }

    // Draw notes
    for (int i = 0; i < visibleSteps; ++i)
    {
        if (!steps[(size_t)i].active) continue;

        int note = steps[(size_t)i].midiNote;
        int relNote = note - baseNote;
        if (relNote < 0 || relNote >= noteRange) continue;

        float x = (float)bounds.getX() + (float)i * stepWidth;
        float y = bounds.getBottom() - (float)(relNote + 1) * noteHeight;

        bool isCurrent = (i == currentStep);
        g.setColour(isCurrent ? OP1Colors::sequencerColor : OP1Colors::sequencerColor.withAlpha(0.7f));
        g.fillRoundedRectangle(x + 0.5f, y, stepWidth - 1.0f, noteHeight, 1.0f);
    }

    // Playhead
    if (currentStep < visibleSteps)
    {
        float px = (float)bounds.getX() + (float)currentStep * stepWidth;
        g.setColour(OP1Colors::textPrimary.withAlpha(0.3f));
        g.drawLine(px, (float)bounds.getY(), px, (float)bounds.getBottom(), 1.0f);
    }
}
