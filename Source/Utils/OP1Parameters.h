#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

namespace OP1Params
{
    // Global mode
    inline const juce::String MODE_ID         = "mode";
    inline const juce::String SYNTH_ENGINE_ID = "synthEngine";
    inline const juce::String EFFECT_TYPE_ID  = "effectType";
    inline const juce::String SEQ_TYPE_ID     = "seqType";

    // Encoder parameters (4 encoders, contextual per mode)
    inline const juce::String ENC1_ID = "encoder1";
    inline const juce::String ENC2_ID = "encoder2";
    inline const juce::String ENC3_ID = "encoder3";
    inline const juce::String ENC4_ID = "encoder4";

    // Synth parameters
    inline const juce::String SYNTH_VOLUME_ID   = "synthVolume";
    inline const juce::String SYNTH_OCTAVE_ID   = "synthOctave";
    inline const juce::String SYNTH_ATTACK_ID   = "synthAttack";
    inline const juce::String SYNTH_DECAY_ID    = "synthDecay";
    inline const juce::String SYNTH_SUSTAIN_ID  = "synthSustain";
    inline const juce::String SYNTH_RELEASE_ID  = "synthRelease";

    // FM Synth
    inline const juce::String FM_RATIO_ID       = "fmRatio";
    inline const juce::String FM_DEPTH_ID       = "fmDepth";
    inline const juce::String FM_FEEDBACK_ID    = "fmFeedback";
    inline const juce::String FM_SHAPE_ID       = "fmShape";

    // Digital Synth
    inline const juce::String DIGI_WAVEFORM_ID  = "digiWaveform";
    inline const juce::String DIGI_DETUNE_ID    = "digiDetune";
    inline const juce::String DIGI_PHASE_ID     = "digiPhase";
    inline const juce::String DIGI_FOLD_ID      = "digiFold";

    // String Synth
    inline const juce::String STR_DAMPING_ID    = "strDamping";
    inline const juce::String STR_BODY_ID       = "strBody";
    inline const juce::String STR_PLUCK_ID      = "strPluck";
    inline const juce::String STR_CHORUS_ID     = "strChorus";

    // Pulse Synth
    inline const juce::String PULSE_WIDTH_ID    = "pulseWidth";
    inline const juce::String PULSE_FINE_ID     = "pulseFine";
    inline const juce::String PULSE_SHAPE_ID    = "pulseShape";
    inline const juce::String PULSE_SYNC_ID     = "pulseSync";

    // Cluster Synth
    inline const juce::String CLUST_SPREAD_ID   = "clustSpread";
    inline const juce::String CLUST_VOICES_ID   = "clustVoices";
    inline const juce::String CLUST_DETUNE_ID   = "clustDetune";
    inline const juce::String CLUST_SHAPE_ID    = "clustShape";

    // DSynth
    inline const juce::String DSYN_ALGORITHM_ID = "dsynAlgorithm";
    inline const juce::String DSYN_PARAM1_ID    = "dsynParam1";
    inline const juce::String DSYN_PARAM2_ID    = "dsynParam2";
    inline const juce::String DSYN_PARAM3_ID    = "dsynParam3";

    // Effects
    inline const juce::String FX_MIX_ID         = "fxMix";
    inline const juce::String FX_PARAM1_ID      = "fxParam1";
    inline const juce::String FX_PARAM2_ID      = "fxParam2";
    inline const juce::String FX_PARAM3_ID      = "fxParam3";

    // Tape
    inline const juce::String TAPE_TRACK_ID     = "tapeTrack";
    inline const juce::String TAPE_SPEED_ID     = "tapeSpeed";
    inline const juce::String TAPE_REVERSE_ID   = "tapeReverse";
    inline const juce::String TAPE_LOOP_IN_ID   = "tapeLoopIn";
    inline const juce::String TAPE_LOOP_OUT_ID  = "tapeLoopOut";

    // Mixer
    inline const juce::String MIX_TRACK1_VOL_ID = "mixTrack1Vol";
    inline const juce::String MIX_TRACK2_VOL_ID = "mixTrack2Vol";
    inline const juce::String MIX_TRACK3_VOL_ID = "mixTrack3Vol";
    inline const juce::String MIX_TRACK4_VOL_ID = "mixTrack4Vol";
    inline const juce::String MIX_MASTER_VOL_ID = "mixMasterVol";
    inline const juce::String MIX_DRIVE_ID      = "mixDrive";
    inline const juce::String MIX_RELEASE_ID    = "mixRelease";
    inline const juce::String MIX_EQ_LOW_ID     = "mixEqLow";
    inline const juce::String MIX_EQ_MID_ID     = "mixEqMid";
    inline const juce::String MIX_EQ_HIGH_ID    = "mixEqHigh";

    // Tempo/transport
    inline const juce::String TEMPO_ID          = "tempo";

    // Enums
    enum class Mode { Synth = 0, Drum, Tape, Mixer };
    enum class SynthEngineType { FM = 0, Digital, String, Pulse, Cluster, DSynth, Sampler };
    enum class EffectType { Nitro = 0, CWO, Delay, Spring, Phone, Punch, None };
    enum class SequencerType { Endless = 0, Pattern, Finger, Sketch, None };

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
}
