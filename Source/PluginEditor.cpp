#include "PluginEditor.h"

OP1FieldEditor::OP1FieldEditor(OP1FieldProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setLookAndFeel(&op1LookAndFeel);

    // Size matching OP-1 Field proportions (wide and thin)
    setSize(900, 520);

    // Display
    addAndMakeVisible(display);

    // Encoders
    addAndMakeVisible(encoder1);
    addAndMakeVisible(encoder2);
    addAndMakeVisible(encoder3);
    addAndMakeVisible(encoder4);

    // Mode buttons
    synthBtn.setActiveColor(OP1Colors::synthColor);
    drumBtn.setActiveColor(OP1Colors::drumColor);
    tapeBtn.setActiveColor(OP1Colors::tapeColor);
    mixerBtn.setActiveColor(OP1Colors::mixerColor);
    seqBtn.setActiveColor(OP1Colors::sequencerColor);
    fxBtn.setActiveColor(OP1Colors::waveformYellow);

    synthBtn.onClick = [this] { setMode(OP1Params::Mode::Synth); };
    drumBtn.onClick = [this] { setMode(OP1Params::Mode::Drum); };
    tapeBtn.onClick = [this] { setMode(OP1Params::Mode::Tape); };
    mixerBtn.onClick = [this] { setMode(OP1Params::Mode::Mixer); };
    seqBtn.onClick = [this] { showSequencer = !showSequencer; showFX = false; updateModeButtons(); };
    fxBtn.onClick = [this] { showFX = !showFX; showSequencer = false; updateModeButtons(); };

    addAndMakeVisible(synthBtn);
    addAndMakeVisible(drumBtn);
    addAndMakeVisible(tapeBtn);
    addAndMakeVisible(mixerBtn);
    addAndMakeVisible(seqBtn);
    addAndMakeVisible(fxBtn);

    // Engine selection buttons
    auto engineButtons = { &engine1Btn, &engine2Btn, &engine3Btn, &engine4Btn,
                           &engine5Btn, &engine6Btn, &engine7Btn, &engine8Btn };
    int engineIdx = 0;
    for (auto* btn : engineButtons)
    {
        addAndMakeVisible(btn);
        int idx = engineIdx;
        btn->onClick = [this, idx] {
            if (showFX) setEffect(idx);
            else setSynthEngine(idx);
        };
        engineIdx++;
    }

    // Transport
    playBtn.onClick = [this] {
        auto* seq = processorRef.getCurrentSequencer();
        if (seq) seq->setPlaying(true);
        processorRef.getTapeRecorder().play();
        playBtn.setActive(true);
        stopBtn.setActive(false);
    };
    stopBtn.onClick = [this] {
        auto* seq = processorRef.getCurrentSequencer();
        if (seq) seq->setPlaying(false);
        processorRef.getTapeRecorder().stop();
        playBtn.setActive(false);
        stopBtn.setActive(true);
    };
    recBtn.onClick = [this] {
        bool isRec = processorRef.getTapeRecorder().isRecording();
        if (!isRec) processorRef.getTapeRecorder().record();
        else processorRef.getTapeRecorder().play();
        recBtn.setActive(!isRec);
    };
    rewBtn.onClick = [this] { processorRef.getTapeRecorder().rewind(); };
    ffBtn.onClick = [this] { processorRef.getTapeRecorder().fastForward(); };

    addAndMakeVisible(playBtn);
    addAndMakeVisible(stopBtn);
    addAndMakeVisible(recBtn);
    addAndMakeVisible(rewBtn);
    addAndMakeVisible(ffBtn);

    // Octave buttons
    octUpBtn.onClick = [this] {
        currentOctave = juce::jmin(currentOctave + 1, 8);
        keyboard.setOctave(currentOctave);
    };
    octDnBtn.onClick = [this] {
        currentOctave = juce::jmax(currentOctave - 1, 0);
        keyboard.setOctave(currentOctave);
    };
    addAndMakeVisible(octUpBtn);
    addAndMakeVisible(octDnBtn);

    // Keyboard
    keyboard.onNoteOn = [this](int note, float vel) {
        auto* engine = processorRef.getCurrentSynthEngine();
        if (engine) engine->noteOn(note, vel);
    };
    keyboard.onNoteOff = [this](int note) {
        auto* engine = processorRef.getCurrentSynthEngine();
        if (engine) engine->noteOff(note);
    };
    addAndMakeVisible(keyboard);

    // VU Meter
    addAndMakeVisible(vuMeter);

    // Initial state
    synthBtn.setActive(true);
    updateModeButtons();
    updateEncoderAttachments();

    // Display content
    display.onPaint = [this](juce::Graphics& g, juce::Rectangle<int> bounds) {
        auto mode = processorRef.getCurrentMode();
        if (showSequencer)
            sequencerScreen.paint(g, bounds);
        else
        {
            switch (mode)
            {
                case OP1Params::Mode::Synth: synthScreen.paint(g, bounds); break;
                case OP1Params::Mode::Drum:  drumScreen.paint(g, bounds); break;
                case OP1Params::Mode::Tape:  tapeScreen.paint(g, bounds); break;
                case OP1Params::Mode::Mixer: mixerScreen.paint(g, bounds); break;
            }
        }
    };

    startTimerHz(30); // UI refresh rate
}

OP1FieldEditor::~OP1FieldEditor()
{
    setLookAndFeel(nullptr);
}

void OP1FieldEditor::setMode(OP1Params::Mode mode)
{
    processorRef.getAPVTS().getParameterAsValue(OP1Params::MODE_ID).setValue((int)mode);
    showSequencer = false;
    showFX = false;
    updateModeButtons();
    updateEncoderAttachments();
}

void OP1FieldEditor::setSynthEngine(int index)
{
    if (index < 7)
        processorRef.getAPVTS().getParameterAsValue(OP1Params::SYNTH_ENGINE_ID).setValue(index);
    updateEncoderAttachments();
}

void OP1FieldEditor::setEffect(int index)
{
    if (index < 7)
        processorRef.getAPVTS().getParameterAsValue(OP1Params::EFFECT_TYPE_ID).setValue(index);
}

void OP1FieldEditor::updateModeButtons()
{
    auto mode = processorRef.getCurrentMode();
    synthBtn.setActive(mode == OP1Params::Mode::Synth && !showSequencer && !showFX);
    drumBtn.setActive(mode == OP1Params::Mode::Drum && !showSequencer && !showFX);
    tapeBtn.setActive(mode == OP1Params::Mode::Tape && !showSequencer && !showFX);
    mixerBtn.setActive(mode == OP1Params::Mode::Mixer && !showSequencer && !showFX);
    seqBtn.setActive(showSequencer);
    fxBtn.setActive(showFX);
    repaint();
}

void OP1FieldEditor::updateEncoderAttachments()
{
    enc1Attach.reset();
    enc2Attach.reset();
    enc3Attach.reset();
    enc4Attach.reset();

    auto& apvts = processorRef.getAPVTS();
    auto mode = processorRef.getCurrentMode();

    if (showFX)
    {
        enc1Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, OP1Params::FX_MIX_ID, encoder1);
        enc2Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, OP1Params::FX_PARAM1_ID, encoder2);
        enc3Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, OP1Params::FX_PARAM2_ID, encoder3);
        enc4Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, OP1Params::FX_PARAM3_ID, encoder4);

        auto* fx = processorRef.getCurrentEffect();
        if (fx)
        {
            display.setEncoderLabels("Mix", fx->getParameterName(0),
                                     fx->getParameterName(1), fx->getParameterName(2));
        }
    }
    else if (mode == OP1Params::Mode::Synth || mode == OP1Params::Mode::Drum)
    {
        auto synthType = processorRef.getCurrentSynthType();
        switch (synthType)
        {
            case OP1Params::SynthEngineType::FM:
                enc1Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                    apvts, OP1Params::FM_RATIO_ID, encoder1);
                enc2Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                    apvts, OP1Params::FM_DEPTH_ID, encoder2);
                enc3Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                    apvts, OP1Params::FM_FEEDBACK_ID, encoder3);
                enc4Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                    apvts, OP1Params::FM_SHAPE_ID, encoder4);
                display.setEncoderLabels("Ratio", "Depth", "Feedback", "Shape");
                break;
            case OP1Params::SynthEngineType::Digital:
                enc1Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                    apvts, OP1Params::DIGI_WAVEFORM_ID, encoder1);
                enc2Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                    apvts, OP1Params::DIGI_DETUNE_ID, encoder2);
                enc3Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                    apvts, OP1Params::DIGI_PHASE_ID, encoder3);
                enc4Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                    apvts, OP1Params::DIGI_FOLD_ID, encoder4);
                display.setEncoderLabels("Wave", "Detune", "Phase", "Fold");
                break;
            case OP1Params::SynthEngineType::String:
                enc1Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                    apvts, OP1Params::STR_DAMPING_ID, encoder1);
                enc2Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                    apvts, OP1Params::STR_BODY_ID, encoder2);
                enc3Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                    apvts, OP1Params::STR_PLUCK_ID, encoder3);
                enc4Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                    apvts, OP1Params::STR_CHORUS_ID, encoder4);
                display.setEncoderLabels("Damping", "Body", "Pluck", "Chorus");
                break;
            case OP1Params::SynthEngineType::Pulse:
                enc1Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                    apvts, OP1Params::PULSE_WIDTH_ID, encoder1);
                enc2Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                    apvts, OP1Params::PULSE_FINE_ID, encoder2);
                enc3Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                    apvts, OP1Params::PULSE_SHAPE_ID, encoder3);
                enc4Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                    apvts, OP1Params::PULSE_SYNC_ID, encoder4);
                display.setEncoderLabels("Width", "Fine", "Shape", "Sync");
                break;
            case OP1Params::SynthEngineType::Cluster:
                enc1Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                    apvts, OP1Params::CLUST_SPREAD_ID, encoder1);
                enc2Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                    apvts, OP1Params::CLUST_VOICES_ID, encoder2);
                enc3Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                    apvts, OP1Params::CLUST_DETUNE_ID, encoder3);
                enc4Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                    apvts, OP1Params::CLUST_SHAPE_ID, encoder4);
                display.setEncoderLabels("Spread", "Voices", "Detune", "Shape");
                break;
            case OP1Params::SynthEngineType::DSynth:
                enc1Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                    apvts, OP1Params::DSYN_ALGORITHM_ID, encoder1);
                enc2Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                    apvts, OP1Params::DSYN_PARAM1_ID, encoder2);
                enc3Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                    apvts, OP1Params::DSYN_PARAM2_ID, encoder3);
                enc4Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                    apvts, OP1Params::DSYN_PARAM3_ID, encoder4);
                display.setEncoderLabels("Algo", "Param 1", "Param 2", "Level");
                break;
            default:
                enc1Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                    apvts, OP1Params::ENC1_ID, encoder1);
                enc2Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                    apvts, OP1Params::ENC2_ID, encoder2);
                enc3Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                    apvts, OP1Params::ENC3_ID, encoder3);
                enc4Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                    apvts, OP1Params::ENC4_ID, encoder4);
                display.setEncoderLabels("P1", "P2", "P3", "P4");
                break;
        }
    }
    else if (mode == OP1Params::Mode::Tape)
    {
        enc1Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, OP1Params::TAPE_LOOP_IN_ID, encoder1);
        enc2Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, OP1Params::TAPE_LOOP_OUT_ID, encoder2);
        enc3Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, OP1Params::TAPE_SPEED_ID, encoder3);
        enc4Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, OP1Params::ENC4_ID, encoder4);
        display.setEncoderLabels("Loop In", "Loop Out", "Speed", "Track");
    }
    else if (mode == OP1Params::Mode::Mixer)
    {
        enc1Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, OP1Params::MIX_MASTER_VOL_ID, encoder1);
        enc2Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, OP1Params::MIX_DRIVE_ID, encoder2);
        enc3Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, OP1Params::MIX_EQ_LOW_ID, encoder3);
        enc4Attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, OP1Params::MIX_EQ_HIGH_ID, encoder4);
        display.setEncoderLabels("Volume", "Drive", "EQ Low", "EQ High");
    }
}

void OP1FieldEditor::updateDisplayContent()
{
    auto mode = processorRef.getCurrentMode();
    auto* engine = processorRef.getCurrentSynthEngine();

    switch (mode)
    {
        case OP1Params::Mode::Synth:
            display.setModeName("SYNTH");
            if (engine) display.setEngineName(engine->getName());
            break;
        case OP1Params::Mode::Drum:
            display.setModeName("DRUM");
            display.setEngineName("Drum Kit");
            break;
        case OP1Params::Mode::Tape:
            display.setModeName("TAPE");
            display.setEngineName("4-Track");
            tapeScreen.setPlaybackPosition(processorRef.getTapeRecorder().getPlaybackPosition());
            tapeScreen.setPlaying(processorRef.getTapeRecorder().isPlaying());
            tapeScreen.setRecording(processorRef.getTapeRecorder().isRecording());
            tapeScreen.setActiveTrack(processorRef.getTapeRecorder().getActiveTrack());
            break;
        case OP1Params::Mode::Mixer:
            display.setModeName("MIXER");
            display.setEngineName("Master");
            break;
    }

    if (showSequencer)
    {
        display.setModeName("SEQ");
        auto* seq = processorRef.getCurrentSequencer();
        if (seq)
        {
            display.setEngineName(seq->getName());
            sequencerScreen.setSequencerName(seq->getName());
            sequencerScreen.setCurrentStep(seq->getCurrentStep());
            sequencerScreen.setPlaying(seq->getPlaying());
        }
    }

    // Update encoder value displays
    display.setEncoderValues(
        (float)encoder1.getValue() / (float)(encoder1.getMaximum() - encoder1.getMinimum()),
        (float)encoder2.getValue() / (float)(encoder2.getMaximum() - encoder2.getMinimum()),
        (float)encoder3.getValue() / (float)(encoder3.getMaximum() - encoder3.getMinimum()),
        (float)encoder4.getValue() / (float)(encoder4.getMaximum() - encoder4.getMinimum())
    );

    float tempo = processorRef.getAPVTS().getRawParameterValue(OP1Params::TEMPO_ID)->load();
    display.setTempo(tempo);
}

void OP1FieldEditor::timerCallback()
{
    updateDisplayContent();
    vuMeter.setLevel(processorRef.getOutputLevelL(), processorRef.getOutputLevelR());
    display.repaint();
}

void OP1FieldEditor::drawBody(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Main body - OP-1 Field aluminum white
    juce::ColourGradient bodyGradient(
        OP1Colors::bodyColor, 0, 0,
        OP1Colors::bodyDark, 0, bounds.getHeight(), false);
    g.setGradientFill(bodyGradient);
    g.fillRoundedRectangle(bounds, 16.0f);

    // Subtle inner border
    g.setColour(juce::Colours::white.withAlpha(0.3f));
    g.drawRoundedRectangle(bounds.reduced(1.0f), 16.0f, 1.0f);

    // Top edge highlight (aluminum sheen)
    juce::ColourGradient topSheen(
        juce::Colours::white.withAlpha(0.15f), 0, 0,
        juce::Colours::transparentWhite, 0, 30.0f, false);
    g.setGradientFill(topSheen);
    g.fillRoundedRectangle(bounds.removeFromTop(30.0f), 16.0f);
}

void OP1FieldEditor::drawScrews(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    float screwRadius = 3.0f;

    // Corner screws (OP-1 Field detail)
    juce::Point<float> screwPositions[] = {
        { 20.0f, 12.0f },
        { (float)bounds.getWidth() - 20.0f, 12.0f },
        { 20.0f, (float)bounds.getHeight() - 12.0f },
        { (float)bounds.getWidth() - 20.0f, (float)bounds.getHeight() - 12.0f }
    };

    for (auto& pos : screwPositions)
    {
        g.setColour(OP1Colors::bodyDark.darker(0.3f));
        g.fillEllipse(pos.x - screwRadius, pos.y - screwRadius,
                       screwRadius * 2, screwRadius * 2);
        g.setColour(OP1Colors::bodyDark);
        g.drawEllipse(pos.x - screwRadius, pos.y - screwRadius,
                       screwRadius * 2, screwRadius * 2, 0.5f);
        // Screw slot
        g.setColour(OP1Colors::bodyDark.darker(0.5f));
        g.drawLine(pos.x - 2, pos.y, pos.x + 2, pos.y, 0.8f);
    }
}

void OP1FieldEditor::drawSpeaker(juce::Graphics& g, juce::Rectangle<int> area)
{
    // OP-1 Field speaker grill pattern
    g.setColour(OP1Colors::bodyDark.darker(0.2f));
    int rows = area.getHeight() / 4;
    int cols = area.getWidth() / 4;
    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
        {
            float x = (float)area.getX() + (float)c * 4.0f + 2.0f;
            float y = (float)area.getY() + (float)r * 4.0f + 2.0f;
            g.fillEllipse(x, y, 1.5f, 1.5f);
        }
    }
}

void OP1FieldEditor::paint(juce::Graphics& g)
{
    // Draw the OP-1 Field body
    drawBody(g);
    drawScrews(g);

    // Speaker grills (left and right of display like OP-1 Field)
    auto bounds = getLocalBounds();
    drawSpeaker(g, juce::Rectangle<int>(12, 30, 40, 80));
    drawSpeaker(g, juce::Rectangle<int>(bounds.getWidth() - 52, 30, 40, 80));

    // OP-1 Field logo area
    g.setColour(OP1Colors::textDim);
    g.setFont(OP1LookAndFeel::getOP1BoldFont(11.0f));
    g.drawText("OP-1 FIELD", bounds.getX() + 60, 6, 100, 16, juce::Justification::centredLeft);

    // Model text
    g.setFont(OP1LookAndFeel::getOP1Font(9.0f));
    g.setColour(OP1Colors::bodyDark.darker(0.3f));
    g.drawText("TEENAGE ENGINEERING", bounds.getWidth() - 170, 6, 150, 14,
               juce::Justification::centredRight);
}

void OP1FieldEditor::resized()
{
    auto bounds = getLocalBounds().reduced(12);

    // Top section: display and encoders
    auto topSection = bounds.removeFromTop(200);

    // Display (center, mimicking the AMOLED screen)
    auto displayArea = topSection.reduced(60, 24);
    displayArea.removeFromBottom(65); // Space for encoders below display
    display.setBounds(displayArea);

    // Encoders below display
    auto encoderSection = topSection.removeFromBottom(65);
    encoderSection.reduce(80, 0);
    int encWidth = encoderSection.getWidth() / 4;
    encoder1.setBounds(encoderSection.removeFromLeft(encWidth).reduced(4));
    encoder2.setBounds(encoderSection.removeFromLeft(encWidth).reduced(4));
    encoder3.setBounds(encoderSection.removeFromLeft(encWidth).reduced(4));
    encoder4.setBounds(encoderSection.removeFromLeft(encWidth).reduced(4));

    // VU Meter (right of display)
    vuMeter.setBounds(displayArea.getRight() + 10, displayArea.getY() + 10,
                      20, displayArea.getHeight() - 20);

    // Middle section: buttons
    bounds.removeFromTop(8);
    auto buttonSection = bounds.removeFromTop(35);

    // Mode buttons (left side)
    auto modeArea = buttonSection.removeFromLeft(280);
    int modeW = modeArea.getWidth() / 6;
    synthBtn.setBounds(modeArea.removeFromLeft(modeW).reduced(2));
    drumBtn.setBounds(modeArea.removeFromLeft(modeW).reduced(2));
    tapeBtn.setBounds(modeArea.removeFromLeft(modeW).reduced(2));
    mixerBtn.setBounds(modeArea.removeFromLeft(modeW).reduced(2));
    seqBtn.setBounds(modeArea.removeFromLeft(modeW).reduced(2));
    fxBtn.setBounds(modeArea.removeFromLeft(modeW).reduced(2));

    // Engine/preset buttons (center)
    buttonSection.removeFromLeft(20);
    auto engineArea = buttonSection.removeFromLeft(240);
    int engW = engineArea.getWidth() / 8;
    engine1Btn.setBounds(engineArea.removeFromLeft(engW).reduced(1));
    engine2Btn.setBounds(engineArea.removeFromLeft(engW).reduced(1));
    engine3Btn.setBounds(engineArea.removeFromLeft(engW).reduced(1));
    engine4Btn.setBounds(engineArea.removeFromLeft(engW).reduced(1));
    engine5Btn.setBounds(engineArea.removeFromLeft(engW).reduced(1));
    engine6Btn.setBounds(engineArea.removeFromLeft(engW).reduced(1));
    engine7Btn.setBounds(engineArea.removeFromLeft(engW).reduced(1));
    engine8Btn.setBounds(engineArea.removeFromLeft(engW).reduced(1));

    // Transport buttons (right side)
    buttonSection.removeFromLeft(20);
    auto transportArea = buttonSection;
    int trW = juce::jmin(transportArea.getWidth() / 5, 55);
    rewBtn.setBounds(transportArea.removeFromLeft(trW).reduced(2));
    ffBtn.setBounds(transportArea.removeFromLeft(trW).reduced(2));
    playBtn.setBounds(transportArea.removeFromLeft(trW).reduced(2));
    stopBtn.setBounds(transportArea.removeFromLeft(trW).reduced(2));
    recBtn.setBounds(transportArea.removeFromLeft(trW).reduced(2));

    // Bottom section: octave buttons and keyboard
    bounds.removeFromTop(8);

    auto octaveArea = bounds.removeFromLeft(40);
    octUpBtn.setBounds(octaveArea.removeFromTop(octaveArea.getHeight() / 2).reduced(2));
    octDnBtn.setBounds(octaveArea.reduced(2));

    keyboard.setBounds(bounds.reduced(4));
}
