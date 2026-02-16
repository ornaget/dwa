#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

// 4-track tape recorder emulation (like OP-1 Field's tape)
class TapeRecorder
{
public:
    TapeRecorder();
    ~TapeRecorder() = default;

    void prepare(double sampleRate, int samplesPerBlock);
    void process(juce::AudioBuffer<float>& inputBuffer, juce::AudioBuffer<float>& outputBuffer);

    // Transport controls
    void play();
    void stop();
    void record();
    void rewind();
    void fastForward();

    // Track management
    void setActiveTrack(int track) { activeTrack = juce::jlimit(0, 3, track); }
    int getActiveTrack() const { return activeTrack; }

    void setTrackVolume(int track, float volume);
    float getTrackVolume(int track) const;

    // Loop points
    void setLoopIn(float pos) { loopInPos = pos; }
    void setLoopOut(float pos) { loopOutPos = pos; }
    void setLoopEnabled(bool enabled) { loopEnabled = enabled; }

    // Tape properties
    void setSpeed(float speed) { tapeSpeed = speed; }
    void setReverse(bool rev) { reverse = rev; }

    // State
    bool isPlaying() const { return playing; }
    bool isRecording() const { return recording; }
    float getPlaybackPosition() const;
    float getTapeLength() const;

    // Get waveform data for display
    const std::vector<float>& getTrackData(int track) const { return tracks[(size_t)track]; }

private:
    static constexpr int numTracks = 4;
    static constexpr int maxTapeLength = 44100 * 60 * 6; // 6 minutes at 44100

    double sampleRate = 44100.0;
    int blockSize = 512;

    // Track buffers
    std::array<std::vector<float>, numTracks> tracks;
    std::array<float, numTracks> trackVolumes = { 0.8f, 0.8f, 0.8f, 0.8f };

    int activeTrack = 0;
    float playHead = 0.0f;
    float tapeSpeed = 1.0f;
    bool reverse = false;
    bool playing = false;
    bool recording = false;
    bool loopEnabled = false;
    float loopInPos = 0.0f;
    float loopOutPos = 1.0f;

    // Tape saturation
    float saturate(float sample);
};
