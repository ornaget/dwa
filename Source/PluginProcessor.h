#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

#include "Audio/Synths/SynthEngine.h"
#include "Audio/Synths/FMSynth.h"
#include "Audio/Synths/DigitalSynth.h"
#include "Audio/Synths/StringSynth.h"
#include "Audio/Synths/PulseSynth.h"
#include "Audio/Synths/ClusterSynth.h"
#include "Audio/Synths/DSynth.h"
#include "Audio/Synths/DrumSampler.h"
#include "Audio/Synths/SamplerEngine.h"

#include "Audio/Effects/EffectProcessor.h"
#include "Audio/Effects/NitroEffect.h"
#include "Audio/Effects/CWOEffect.h"
#include "Audio/Effects/DelayEffect.h"
#include "Audio/Effects/SpringEffect.h"
#include "Audio/Effects/PhoneEffect.h"
#include "Audio/Effects/PunchEffect.h"

#include "Audio/Tape/TapeRecorder.h"

#include "Audio/Sequencers/SequencerBase.h"
#include "Audio/Sequencers/EndlessSequencer.h"
#include "Audio/Sequencers/PatternSequencer.h"
#include "Audio/Sequencers/FingerSequencer.h"
#include "Audio/Sequencers/SketchSequencer.h"

#include "Utils/OP1Parameters.h"

class OP1FieldProcessor : public juce::AudioProcessor
{
public:
    OP1FieldProcessor();
    ~OP1FieldProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "OP-1 Field Emulator"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 2.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Public accessors for the editor
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    SynthEngine* getCurrentSynthEngine();
    EffectProcessor* getCurrentEffect();
    SequencerBase* getCurrentSequencer();
    TapeRecorder& getTapeRecorder() { return tapeRecorder; }

    OP1Params::Mode getCurrentMode() const { return currentMode; }
    OP1Params::SynthEngineType getCurrentSynthType() const { return currentSynthType; }
    OP1Params::EffectType getCurrentEffectType() const { return currentEffectType; }

    // Level metering
    float getOutputLevelL() const { return outputLevelL.load(); }
    float getOutputLevelR() const { return outputLevelR.load(); }

private:
    juce::AudioProcessorValueTreeState apvts;

    // Current state
    OP1Params::Mode currentMode = OP1Params::Mode::Synth;
    OP1Params::SynthEngineType currentSynthType = OP1Params::SynthEngineType::FM;
    OP1Params::EffectType currentEffectType = OP1Params::EffectType::None;
    OP1Params::SequencerType currentSeqType = OP1Params::SequencerType::None;

    // Synth engines
    FMSynth fmSynth;
    DigitalSynth digitalSynth;
    StringSynth stringSynth;
    PulseSynth pulseSynth;
    ClusterSynth clusterSynth;
    DSynth dsynth;
    DrumSampler drumSampler;
    SamplerEngine samplerEngine;

    // Effects
    NitroEffect nitroEffect;
    CWOEffect cwoEffect;
    DelayEffect delayEffect;
    SpringEffect springEffect;
    PhoneEffect phoneEffect;
    PunchEffect punchEffect;

    // Tape
    TapeRecorder tapeRecorder;

    // Sequencers
    EndlessSequencer endlessSeq;
    PatternSequencer patternSeq;
    FingerSequencer fingerSeq;
    SketchSequencer sketchSeq;

    // Metering
    std::atomic<float> outputLevelL { 0.0f };
    std::atomic<float> outputLevelR { 0.0f };

    // Master processing
    float masterVolume = 0.8f;
    float masterDrive = 0.0f;

    void updateParametersFromAPVTS();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OP1FieldProcessor)
};
