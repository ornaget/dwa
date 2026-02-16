#include "OP1Keyboard.h"
#include "../LookAndFeel/OP1LookAndFeel.h"

OP1Keyboard::OP1Keyboard()
{
}

void OP1Keyboard::updateKeyBounds()
{
    auto bounds = getLocalBounds().toFloat().reduced(4.0f);
    float whiteKeyWidth = bounds.getWidth() / 14.0f; // 14 white keys in 2 octaves
    float blackKeyWidth = whiteKeyWidth * 0.65f;
    float whiteKeyHeight = bounds.getHeight();
    float blackKeyHeight = whiteKeyHeight * 0.55f;

    // White key pattern per octave: C D E F G A B
    bool isBlackKey[] = { false, true, false, true, false, false, true, false, true, false, true, false };
    int whiteIndex = 0;

    for (int i = 0; i < numKeys; ++i)
    {
        int noteInOctave = i % 12;
        keys[(size_t)i].isBlack = isBlackKey[noteInOctave];
        keys[(size_t)i].noteOffset = i;

        if (!keys[(size_t)i].isBlack)
        {
            keys[(size_t)i].bounds = juce::Rectangle<float>(
                bounds.getX() + (float)whiteIndex * whiteKeyWidth,
                bounds.getY(),
                whiteKeyWidth - 1.0f,  // 1px gap between white keys
                whiteKeyHeight);
            whiteIndex++;
        }
    }

    // Position black keys over white keys
    whiteIndex = 0;
    for (int i = 0; i < numKeys; ++i)
    {
        int noteInOctave = i % 12;
        if (!isBlackKey[noteInOctave])
        {
            whiteIndex++;
        }
        else
        {
            float xPos = bounds.getX() + ((float)whiteIndex - 0.5f) * whiteKeyWidth
                         - blackKeyWidth * 0.5f + whiteKeyWidth * 0.5f;
            keys[(size_t)i].bounds = juce::Rectangle<float>(
                xPos, bounds.getY(), blackKeyWidth, blackKeyHeight);
        }
    }
}

void OP1Keyboard::resized()
{
    updateKeyBounds();
}

void OP1Keyboard::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Background: body color with very subtle rounded rectangle
    g.setColour(OP1Colors::body);
    g.fillRoundedRectangle(bounds, 6.0f);

    // Draw white keys first
    for (int i = 0; i < numKeys; ++i)
    {
        if (keys[(size_t)i].isBlack) continue;

        auto key = keys[(size_t)i];
        bool isPressed = (i == pressedKey);

        // Key body - flat OP-1 Field style with 4px rounded rectangle
        g.setColour(isPressed ? OP1Colors::keyActive : OP1Colors::keyWhite);
        g.fillRoundedRectangle(key.bounds, 4.0f);

        // Subtle keyWhiteEdge bottom border for depth
        if (!isPressed)
        {
            auto edgeBounds = key.bounds;
            edgeBounds.removeFromTop(edgeBounds.getHeight() - 2.0f);
            g.setColour(OP1Colors::keyWhiteEdge);
            // Use a path to only round bottom corners
            juce::Path edgePath;
            edgePath.addRoundedRectangle(edgeBounds.getX(), edgeBounds.getY(),
                                          edgeBounds.getWidth(), edgeBounds.getHeight(),
                                          4.0f, 4.0f,
                                          false, false, true, true);
            g.fillPath(edgePath);
        }

        // C note labels
        if (key.noteOffset % 12 == 0)
        {
            g.setColour(OP1Colors::textSecondary);
            g.setFont(OP1LookAndFeel::getDisplayFont(9.0f));
            int octave = currentOctave + key.noteOffset / 12;
            auto labelBounds = key.bounds;
            labelBounds = labelBounds.removeFromBottom(14.0f);
            g.drawText("C" + juce::String(octave), labelBounds,
                       juce::Justification::centred);
        }
    }

    // Draw black keys on top
    for (int i = 0; i < numKeys; ++i)
    {
        if (!keys[(size_t)i].isBlack) continue;

        auto key = keys[(size_t)i];
        bool isPressed = (i == pressedKey);

        // Shadow on bottom only (not offset)
        g.setColour(juce::Colours::black.withAlpha(0.2f));
        auto shadowBounds = key.bounds;
        shadowBounds.removeFromTop(shadowBounds.getHeight() - 3.0f);
        g.fillRoundedRectangle(shadowBounds.expanded(0.5f, 0.0f).translated(0.0f, 1.0f), 2.0f);

        // Key body - 3px rounded
        g.setColour(isPressed ? OP1Colors::keyActive : OP1Colors::keyBlack);
        g.fillRoundedRectangle(key.bounds, 3.0f);

        // Top area gets subtle white 4% highlight
        if (!isPressed)
        {
            auto highlightBounds = key.bounds.reduced(1.0f);
            highlightBounds.setHeight(highlightBounds.getHeight() * 0.35f);
            g.setColour(juce::Colours::white.withAlpha(0.04f));
            juce::Path highlightPath;
            highlightPath.addRoundedRectangle(highlightBounds.getX(), highlightBounds.getY(),
                                               highlightBounds.getWidth(), highlightBounds.getHeight(),
                                               3.0f, 3.0f,
                                               true, true, false, false);
            g.fillPath(highlightPath);
        }
    }
}

int OP1Keyboard::getKeyAtPosition(juce::Point<float> pos)
{
    // Check black keys first (they're on top)
    for (int i = numKeys - 1; i >= 0; --i)
        if (keys[(size_t)i].isBlack && keys[(size_t)i].bounds.contains(pos))
            return i;
    // Then white keys
    for (int i = 0; i < numKeys; ++i)
        if (!keys[(size_t)i].isBlack && keys[(size_t)i].bounds.contains(pos))
            return i;
    return -1;
}

int OP1Keyboard::keyToMidiNote(int key)
{
    return currentOctave * 12 + keys[(size_t)key].noteOffset;
}

void OP1Keyboard::mouseDown(const juce::MouseEvent& event)
{
    int key = getKeyAtPosition(event.position);
    if (key >= 0)
    {
        pressedKey = key;
        int midiNote = keyToMidiNote(key);
        float velocity = juce::jlimit(0.0f, 1.0f, event.position.y / (float)getHeight());
        if (onNoteOn) onNoteOn(midiNote, velocity);
        repaint();
    }
}

void OP1Keyboard::mouseUp(const juce::MouseEvent& /*event*/)
{
    if (pressedKey >= 0)
    {
        int midiNote = keyToMidiNote(pressedKey);
        if (onNoteOff) onNoteOff(midiNote);
        pressedKey = -1;
        repaint();
    }
}

void OP1Keyboard::mouseDrag(const juce::MouseEvent& event)
{
    int key = getKeyAtPosition(event.position);
    if (key != pressedKey)
    {
        if (pressedKey >= 0)
        {
            int midiNote = keyToMidiNote(pressedKey);
            if (onNoteOff) onNoteOff(midiNote);
        }
        pressedKey = key;
        if (key >= 0)
        {
            int midiNote = keyToMidiNote(key);
            float velocity = juce::jlimit(0.0f, 1.0f, event.position.y / (float)getHeight());
            if (onNoteOn) onNoteOn(midiNote, velocity);
        }
        repaint();
    }
}
