#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../../Utils/OP1Colors.h"

class DrumScreen
{
public:
    DrumScreen() = default;
    void paint(juce::Graphics& g, juce::Rectangle<int> bounds);

    void setActiveSlot(int slot) { activeSlot = slot; }
    void setSlotLevel(int slot, float level);
    void triggerSlot(int slot);

private:
    int activeSlot = 0;
    float slotLevels[8] = {};
    float slotTriggers[8] = {};

    static constexpr const char* drumNames[] = {
        "KICK", "SNARE", "HIHAT", "CLAP", "TOM", "RIM", "BELL", "CYM"
    };
};
