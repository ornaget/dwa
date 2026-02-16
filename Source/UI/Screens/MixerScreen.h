#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../../Utils/OP1Colors.h"

class MixerScreen
{
public:
    MixerScreen() = default;
    void paint(juce::Graphics& g, juce::Rectangle<int> bounds);

    void setTrackVolume(int track, float volume);
    void setTrackLevel(int track, float level);
    void setMasterVolume(float vol) { masterVolume = vol; }
    void setEQ(float low, float mid, float high) { eqLow = low; eqMid = mid; eqHigh = high; }
    void setDrive(float d) { drive = d; }

private:
    float trackVolumes[4] = { 0.8f, 0.8f, 0.8f, 0.8f };
    float trackLevels[4] = {};
    float masterVolume = 0.8f;
    float drive = 0.0f;
    float eqLow = 0.0f, eqMid = 0.0f, eqHigh = 0.0f;

    void drawChannel(juce::Graphics& g, juce::Rectangle<int> bounds, int track, juce::Colour color);
    void drawMaster(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawEQ(juce::Graphics& g, juce::Rectangle<int> bounds);
};
