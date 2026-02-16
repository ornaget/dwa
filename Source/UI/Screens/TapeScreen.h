#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../../Utils/OP1Colors.h"

class TapeScreen
{
public:
    TapeScreen() = default;
    void paint(juce::Graphics& g, juce::Rectangle<int> bounds);

    void setPlaybackPosition(float pos) { playPosition = pos; }
    void setActiveTrack(int track) { activeTrack = track; }
    void setPlaying(bool p) { isPlaying = p; }
    void setRecording(bool r) { isRecording = r; }
    void setLoopRegion(float start, float end) { loopStart = start; loopEnd = end; }
    void setTrackData(int track, const std::vector<float>& data);

private:
    float playPosition = 0.0f;
    int activeTrack = 0;
    bool isPlaying = false;
    bool isRecording = false;
    float loopStart = 0.0f;
    float loopEnd = 1.0f;

    std::vector<float> trackWaveforms[4]; // simplified waveform data for display
    float tapeReelAngle = 0.0f;

    void drawTapeReels(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawTracks(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawPlayhead(juce::Graphics& g, juce::Rectangle<int> bounds);
    void drawTransportInfo(juce::Graphics& g, juce::Rectangle<int> bounds);
};
