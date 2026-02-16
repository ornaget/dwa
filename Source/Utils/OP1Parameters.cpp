#include "OP1Parameters.h"

juce::AudioProcessorValueTreeState::ParameterLayout OP1Params::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Global mode
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        MODE_ID, "Mode", juce::StringArray{"Synth", "Drum", "Tape", "Mixer"}, 0));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        SYNTH_ENGINE_ID, "Synth Engine",
        juce::StringArray{"FM", "Digital", "String", "Pulse", "Cluster", "DSynth", "Sampler"}, 0));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        EFFECT_TYPE_ID, "Effect Type",
        juce::StringArray{"Nitro", "CWO", "Delay", "Spring", "Phone", "Punch", "None"}, 6));
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        SEQ_TYPE_ID, "Sequencer Type",
        juce::StringArray{"Endless", "Pattern", "Finger", "Sketch", "None"}, 4));

    // Four encoders (normalized 0-1, meaning depends on context)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ENC1_ID, "Encoder 1", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ENC2_ID, "Encoder 2", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ENC3_ID, "Encoder 3", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ENC4_ID, "Encoder 4", 0.0f, 1.0f, 0.5f));

    // Synth ADSR
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        SYNTH_VOLUME_ID, "Synth Volume", 0.0f, 1.0f, 0.8f));
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        SYNTH_OCTAVE_ID, "Octave", -3, 3, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        SYNTH_ATTACK_ID, "Attack",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f), 0.01f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        SYNTH_DECAY_ID, "Decay",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f), 0.3f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        SYNTH_SUSTAIN_ID, "Sustain", 0.0f, 1.0f, 0.7f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        SYNTH_RELEASE_ID, "Release",
        juce::NormalisableRange<float>(0.001f, 10.0f, 0.001f, 0.3f), 0.5f));

    // FM Synth
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        FM_RATIO_ID, "FM Ratio",
        juce::NormalisableRange<float>(0.5f, 16.0f, 0.01f, 0.5f), 2.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        FM_DEPTH_ID, "FM Depth", 0.0f, 1.0f, 0.3f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        FM_FEEDBACK_ID, "FM Feedback", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        FM_SHAPE_ID, "FM Shape", 0.0f, 1.0f, 0.0f));

    // Digital Synth
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        DIGI_WAVEFORM_ID, "Waveform", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        DIGI_DETUNE_ID, "Detune", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        DIGI_PHASE_ID, "Phase", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        DIGI_FOLD_ID, "Fold", 0.0f, 1.0f, 0.0f));

    // String Synth
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        STR_DAMPING_ID, "Damping", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        STR_BODY_ID, "Body", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        STR_PLUCK_ID, "Pluck", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        STR_CHORUS_ID, "String Chorus", 0.0f, 1.0f, 0.3f));

    // Pulse Synth
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PULSE_WIDTH_ID, "Pulse Width", 0.01f, 0.99f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PULSE_FINE_ID, "Fine Tune", -1.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PULSE_SHAPE_ID, "Pulse Shape", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        PULSE_SYNC_ID, "Sync", 0.0f, 1.0f, 0.0f));

    // Cluster Synth
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        CLUST_SPREAD_ID, "Spread", 0.0f, 1.0f, 0.3f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        CLUST_VOICES_ID, "Voices",
        juce::NormalisableRange<float>(1.0f, 8.0f, 1.0f), 4.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        CLUST_DETUNE_ID, "Cluster Detune", 0.0f, 1.0f, 0.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        CLUST_SHAPE_ID, "Cluster Shape", 0.0f, 1.0f, 0.0f));

    // DSynth
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        DSYN_ALGORITHM_ID, "Algorithm",
        juce::NormalisableRange<float>(0.0f, 7.0f, 1.0f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        DSYN_PARAM1_ID, "DSynth P1", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        DSYN_PARAM2_ID, "DSynth P2", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        DSYN_PARAM3_ID, "DSynth P3", 0.0f, 1.0f, 0.5f));

    // Effects
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        FX_MIX_ID, "FX Mix", 0.0f, 1.0f, 0.3f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        FX_PARAM1_ID, "FX Param 1", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        FX_PARAM2_ID, "FX Param 2", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        FX_PARAM3_ID, "FX Param 3", 0.0f, 1.0f, 0.5f));

    // Tape
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        TAPE_TRACK_ID, "Tape Track", 1, 4, 1));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        TAPE_SPEED_ID, "Tape Speed",
        juce::NormalisableRange<float>(0.25f, 4.0f, 0.01f, 0.5f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        TAPE_REVERSE_ID, "Tape Reverse", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        TAPE_LOOP_IN_ID, "Loop In", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        TAPE_LOOP_OUT_ID, "Loop Out", 0.0f, 1.0f, 1.0f));

    // Mixer
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        MIX_TRACK1_VOL_ID, "Track 1 Vol", 0.0f, 1.0f, 0.8f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        MIX_TRACK2_VOL_ID, "Track 2 Vol", 0.0f, 1.0f, 0.8f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        MIX_TRACK3_VOL_ID, "Track 3 Vol", 0.0f, 1.0f, 0.8f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        MIX_TRACK4_VOL_ID, "Track 4 Vol", 0.0f, 1.0f, 0.8f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        MIX_MASTER_VOL_ID, "Master Vol", 0.0f, 1.0f, 0.8f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        MIX_DRIVE_ID, "Drive", 0.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        MIX_RELEASE_ID, "Mix Release", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        MIX_EQ_LOW_ID, "EQ Low", -1.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        MIX_EQ_MID_ID, "EQ Mid", -1.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        MIX_EQ_HIGH_ID, "EQ High", -1.0f, 1.0f, 0.0f));

    // Tempo
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        TEMPO_ID, "Tempo",
        juce::NormalisableRange<float>(40.0f, 240.0f, 0.1f), 120.0f));

    return { params.begin(), params.end() };
}
