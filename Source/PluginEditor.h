#pragma once
#include "PluginProcessor.h"
#include "UI/LookAndFeel/OP1LookAndFeel.h"
#include "UI/Components/OP1Display.h"
#include "UI/Components/OP1Encoder.h"
#include "UI/Components/OP1Keyboard.h"
#include "UI/Components/OP1Button.h"
#include "UI/Components/VUMeter.h"
#include "UI/Screens/SynthScreen.h"
#include "UI/Screens/DrumScreen.h"
#include "UI/Screens/TapeScreen.h"
#include "UI/Screens/MixerScreen.h"
#include "UI/Screens/SequencerScreen.h"

class OP1FieldEditor : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    explicit OP1FieldEditor(OP1FieldProcessor& processor);
    ~OP1FieldEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

private:
    OP1FieldProcessor& processorRef;
    OP1LookAndFeel op1LookAndFeel;

    // Main display
    OP1Display display;

    // Four encoders (Blue, Green, White, Orange - like OP-1 Field)
    OP1Encoder encoder1 { "Enc1", OP1Colors::encoderBlue };
    OP1Encoder encoder2 { "Enc2", OP1Colors::encoderGreen };
    OP1Encoder encoder3 { "Enc3", OP1Colors::encoderWhite };
    OP1Encoder encoder4 { "Enc4", OP1Colors::encoderOrange };

    // Mode buttons
    OP1Button synthBtn { "SYNTH", OP1Button::Style::Mode };
    OP1Button drumBtn  { "DRUM",  OP1Button::Style::Mode };
    OP1Button tapeBtn  { "TAPE",  OP1Button::Style::Mode };
    OP1Button mixerBtn { "MIXER", OP1Button::Style::Mode };

    // Engine selection buttons
    OP1Button engine1Btn { "1", OP1Button::Style::Small };
    OP1Button engine2Btn { "2", OP1Button::Style::Small };
    OP1Button engine3Btn { "3", OP1Button::Style::Small };
    OP1Button engine4Btn { "4", OP1Button::Style::Small };
    OP1Button engine5Btn { "5", OP1Button::Style::Small };
    OP1Button engine6Btn { "6", OP1Button::Style::Small };
    OP1Button engine7Btn { "7", OP1Button::Style::Small };
    OP1Button engine8Btn { "8", OP1Button::Style::Small };

    // Transport buttons
    OP1Button playBtn   { "PLAY",   OP1Button::Style::Transport };
    OP1Button stopBtn   { "STOP",   OP1Button::Style::Transport };
    OP1Button recBtn    { "REC",    OP1Button::Style::Transport };
    OP1Button rewBtn    { "<<",     OP1Button::Style::Transport };
    OP1Button ffBtn     { ">>",     OP1Button::Style::Transport };

    // Misc buttons
    OP1Button seqBtn    { "SEQ",    OP1Button::Style::Mode };
    OP1Button fxBtn     { "FX",     OP1Button::Style::Mode };
    OP1Button octUpBtn  { "OCT+",   OP1Button::Style::Small };
    OP1Button octDnBtn  { "OCT-",   OP1Button::Style::Small };

    // Keyboard
    OP1Keyboard keyboard;

    // VU Meter
    VUMeter vuMeter;

    // Screens
    SynthScreen synthScreen;
    DrumScreen drumScreen;
    TapeScreen tapeScreen;
    MixerScreen mixerScreen;
    SequencerScreen sequencerScreen;

    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> enc1Attach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> enc2Attach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> enc3Attach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> enc4Attach;

    // State
    int currentOctave = 4;
    bool showSequencer = false;
    bool showFX = false;

    void updateModeButtons();
    void updateDisplayContent();
    void updateEncoderAttachments();
    void setMode(OP1Params::Mode mode);
    void setSynthEngine(int index);
    void setEffect(int index);

    void drawBody(juce::Graphics& g);
    void drawScrews(juce::Graphics& g);
    void drawSpeaker(juce::Graphics& g, juce::Rectangle<int> area);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OP1FieldEditor)
};
