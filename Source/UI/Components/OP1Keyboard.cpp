#include "OP1Keyboard.h"

OP1Keyboard::OP1Keyboard()
{
}

void OP1Keyboard::updateKeyBounds()
{
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);
    float whiteKeyWidth = bounds.getWidth() / 14.0f; // 14 white keys in 2 octaves
    float blackKeyWidth = whiteKeyWidth * 0.65f;
    float whiteKeyHeight = bounds.getHeight();
    float blackKeyHeight = whiteKeyHeight * 0.6f;

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
                whiteKeyWidth - 1.0f,
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

    // Background
    g.setColour(OP1Colors::bodyDark);
    g.fillRoundedRectangle(bounds, 4.0f);

    // Draw white keys first
    for (int i = 0; i < numKeys; ++i)
    {
        if (keys[(size_t)i].isBlack) continue;

        auto& key = keys[(size_t)i];
        bool isPressed = (i == pressedKey);

        // Key body with OP-1 style rounded rectangles
        g.setColour(isPressed ? OP1Colors::keyPressed : OP1Colors::keyWhite);
        g.fillRoundedRectangle(key.bounds, 3.0f);

        // Subtle border
        g.setColour(OP1Colors::bodyDark.withAlpha(0.3f));
        g.drawRoundedRectangle(key.bounds, 3.0f, 0.5f);

        // Key label for C notes
        if (key.noteOffset % 12 == 0)
        {
            g.setColour(OP1Colors::textDim);
            g.setFont(juce::Font(juce::FontOptions(10.0f)));
            int octave = currentOctave + key.noteOffset / 12;
            g.drawText("C" + juce::String(octave),
                       key.bounds.removeFromBottom(15.0f),
                       juce::Justification::centred);
        }
    }

    // Draw black keys on top
    for (int i = 0; i < numKeys; ++i)
    {
        if (!keys[(size_t)i].isBlack) continue;

        auto& key = keys[(size_t)i];
        bool isPressed = (i == pressedKey);

        // Shadow
        g.setColour(juce::Colours::black.withAlpha(0.3f));
        g.fillRoundedRectangle(key.bounds.translated(1, 1), 2.0f);

        // Key body
        g.setColour(isPressed ? OP1Colors::keyPressed : OP1Colors::keyBlack);
        g.fillRoundedRectangle(key.bounds, 2.0f);

        // Highlight
        g.setColour(juce::Colours::white.withAlpha(0.05f));
        auto highlightBounds = key.bounds.reduced(2.0f);
        highlightBounds.setHeight(highlightBounds.getHeight() * 0.3f);
        g.fillRoundedRectangle(highlightBounds, 1.0f);
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
