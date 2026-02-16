#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "../../Utils/OP1Colors.h"

// OP-1 Field keyboard representation
class OP1Keyboard : public juce::Component
{
public:
    OP1Keyboard();
    ~OP1Keyboard() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;

    void setOctave(int octave) { currentOctave = octave; repaint(); }
    int getOctave() const { return currentOctave; }

    std::function<void(int midiNote, float velocity)> onNoteOn;
    std::function<void(int midiNote)> onNoteOff;

private:
    static constexpr int numKeys = 24; // 2 octaves visible like OP-1
    int currentOctave = 4;
    int pressedKey = -1;
    int hoverKey = -1;

    struct KeyInfo
    {
        juce::Rectangle<float> bounds;
        bool isBlack = false;
        int noteOffset = 0;
    };

    std::array<KeyInfo, numKeys> keys;
    void updateKeyBounds();
    int getKeyAtPosition(juce::Point<float> pos);
    int keyToMidiNote(int key);
};
