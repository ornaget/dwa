cmake_minimum_required(VERSION 3.22)
project(OP1FieldEmulator VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Suppress deprecation warnings on macOS 15+
add_compile_options(-Wno-deprecated-declarations)

# Fetch JUCE
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.4
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

# Plugin target
juce_add_plugin(OP1FieldEmulator
    COMPANY_NAME "OP1Emu"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Op1E
    PLUGIN_CODE Op1F
    FORMATS AU VST3 Standalone
    PRODUCT_NAME "OP-1 Field Emulator"
    BUNDLE_ID "com.op1emu.op1field"
    AU_MAIN_TYPE "kAudioUnitType_MusicDevice"
)

target_sources(OP1FieldEmulator
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h

        # Audio - Synth Engines
        Source/Audio/Synths/SynthEngine.h
        Source/Audio/Synths/SynthEngine.cpp
        Source/Audio/Synths/FMSynth.h
        Source/Audio/Synths/FMSynth.cpp
        Source/Audio/Synths/DigitalSynth.h
        Source/Audio/Synths/DigitalSynth.cpp
        Source/Audio/Synths/StringSynth.h
        Source/Audio/Synths/StringSynth.cpp
        Source/Audio/Synths/PulseSynth.h
        Source/Audio/Synths/PulseSynth.cpp
        Source/Audio/Synths/ClusterSynth.h
        Source/Audio/Synths/ClusterSynth.cpp
        Source/Audio/Synths/DSynth.h
        Source/Audio/Synths/DSynth.cpp
        Source/Audio/Synths/DrumSampler.h
        Source/Audio/Synths/DrumSampler.cpp
        Source/Audio/Synths/SamplerEngine.h
        Source/Audio/Synths/SamplerEngine.cpp

        # Audio - Effects
        Source/Audio/Effects/EffectProcessor.h
        Source/Audio/Effects/EffectProcessor.cpp
        Source/Audio/Effects/NitroEffect.h
        Source/Audio/Effects/NitroEffect.cpp
        Source/Audio/Effects/CWOEffect.h
        Source/Audio/Effects/CWOEffect.cpp
        Source/Audio/Effects/DelayEffect.h
        Source/Audio/Effects/DelayEffect.cpp
        Source/Audio/Effects/SpringEffect.h
        Source/Audio/Effects/SpringEffect.cpp
        Source/Audio/Effects/PhoneEffect.h
        Source/Audio/Effects/PhoneEffect.cpp
        Source/Audio/Effects/PunchEffect.h
        Source/Audio/Effects/PunchEffect.cpp

        # Audio - Tape
        Source/Audio/Tape/TapeRecorder.h
        Source/Audio/Tape/TapeRecorder.cpp

        # Audio - Sequencers
        Source/Audio/Sequencers/SequencerBase.h
        Source/Audio/Sequencers/SequencerBase.cpp
        Source/Audio/Sequencers/EndlessSequencer.h
        Source/Audio/Sequencers/EndlessSequencer.cpp
        Source/Audio/Sequencers/PatternSequencer.h
        Source/Audio/Sequencers/PatternSequencer.cpp
        Source/Audio/Sequencers/FingerSequencer.h
        Source/Audio/Sequencers/FingerSequencer.cpp
        Source/Audio/Sequencers/SketchSequencer.h
        Source/Audio/Sequencers/SketchSequencer.cpp

        # UI
        Source/UI/LookAndFeel/OP1LookAndFeel.h
        Source/UI/LookAndFeel/OP1LookAndFeel.cpp
        Source/UI/Components/OP1Display.h
        Source/UI/Components/OP1Display.cpp
        Source/UI/Components/OP1Encoder.h
        Source/UI/Components/OP1Encoder.cpp
        Source/UI/Components/OP1Keyboard.h
        Source/UI/Components/OP1Keyboard.cpp
        Source/UI/Components/OP1Button.h
        Source/UI/Components/OP1Button.cpp
        Source/UI/Components/VUMeter.h
        Source/UI/Components/VUMeter.cpp
        Source/UI/Screens/SynthScreen.h
        Source/UI/Screens/SynthScreen.cpp
        Source/UI/Screens/DrumScreen.h
        Source/UI/Screens/DrumScreen.cpp
        Source/UI/Screens/TapeScreen.h
        Source/UI/Screens/TapeScreen.cpp
        Source/UI/Screens/MixerScreen.h
        Source/UI/Screens/MixerScreen.cpp
        Source/UI/Screens/SequencerScreen.h
        Source/UI/Screens/SequencerScreen.cpp

        # Utils
        Source/Utils/OP1Parameters.h
        Source/Utils/OP1Parameters.cpp
        Source/Utils/OP1Colors.h
)

target_compile_definitions(OP1FieldEmulator
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(OP1FieldEmulator
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
