#include "PluginProcessor.h"
#include "PluginEditor.h"

OP1FieldProcessor::OP1FieldProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "OP1Parameters", OP1Params::createParameterLayout())
{
}

OP1FieldProcessor::~OP1FieldProcessor() = default;

void OP1FieldProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Prepare all synth engines
    fmSynth.prepare(sampleRate, samplesPerBlock);
    digitalSynth.prepare(sampleRate, samplesPerBlock);
    stringSynth.prepare(sampleRate, samplesPerBlock);
    pulseSynth.prepare(sampleRate, samplesPerBlock);
    clusterSynth.prepare(sampleRate, samplesPerBlock);
    dsynth.prepare(sampleRate, samplesPerBlock);
    drumSampler.prepare(sampleRate, samplesPerBlock);
    samplerEngine.prepare(sampleRate, samplesPerBlock);

    // Prepare effects
    nitroEffect.prepare(sampleRate, samplesPerBlock);
    cwoEffect.prepare(sampleRate, samplesPerBlock);
    delayEffect.prepare(sampleRate, samplesPerBlock);
    springEffect.prepare(sampleRate, samplesPerBlock);
    phoneEffect.prepare(sampleRate, samplesPerBlock);
    punchEffect.prepare(sampleRate, samplesPerBlock);

    // Prepare tape
    tapeRecorder.prepare(sampleRate, samplesPerBlock);

    // Prepare sequencers
    endlessSeq.prepare(sampleRate);
    patternSeq.prepare(sampleRate);
    fingerSeq.prepare(sampleRate);
    sketchSeq.prepare(sampleRate);
}

void OP1FieldProcessor::releaseResources()
{
}

void OP1FieldProcessor::updateParametersFromAPVTS()
{
    // Mode
    auto* modeParam = apvts.getRawParameterValue(OP1Params::MODE_ID);
    currentMode = static_cast<OP1Params::Mode>((int)modeParam->load());

    // Synth engine type
    auto* synthParam = apvts.getRawParameterValue(OP1Params::SYNTH_ENGINE_ID);
    currentSynthType = static_cast<OP1Params::SynthEngineType>((int)synthParam->load());

    // Effect type
    auto* fxParam = apvts.getRawParameterValue(OP1Params::EFFECT_TYPE_ID);
    currentEffectType = static_cast<OP1Params::EffectType>((int)fxParam->load());

    // Sequencer type
    auto* seqParam = apvts.getRawParameterValue(OP1Params::SEQ_TYPE_ID);
    currentSeqType = static_cast<OP1Params::SequencerType>((int)seqParam->load());

    // ADSR
    float attack = apvts.getRawParameterValue(OP1Params::SYNTH_ATTACK_ID)->load();
    float decay_ = apvts.getRawParameterValue(OP1Params::SYNTH_DECAY_ID)->load();
    float sustain = apvts.getRawParameterValue(OP1Params::SYNTH_SUSTAIN_ID)->load();
    float release = apvts.getRawParameterValue(OP1Params::SYNTH_RELEASE_ID)->load();

    auto* engine = getCurrentSynthEngine();
    if (engine) engine->setADSR(attack, decay_, sustain, release);

    // Effect mix and params
    auto* effect = getCurrentEffect();
    if (effect)
    {
        effect->setMix(apvts.getRawParameterValue(OP1Params::FX_MIX_ID)->load());
        effect->setParameter(0, apvts.getRawParameterValue(OP1Params::FX_PARAM1_ID)->load());
        effect->setParameter(1, apvts.getRawParameterValue(OP1Params::FX_PARAM2_ID)->load());
        effect->setParameter(2, apvts.getRawParameterValue(OP1Params::FX_PARAM3_ID)->load());
    }

    // Synth-specific params
    switch (currentSynthType)
    {
        case OP1Params::SynthEngineType::FM:
            fmSynth.setParameter(0, apvts.getRawParameterValue(OP1Params::FM_RATIO_ID)->load() / 16.0f);
            fmSynth.setParameter(1, apvts.getRawParameterValue(OP1Params::FM_DEPTH_ID)->load());
            fmSynth.setParameter(2, apvts.getRawParameterValue(OP1Params::FM_FEEDBACK_ID)->load());
            fmSynth.setParameter(3, apvts.getRawParameterValue(OP1Params::FM_SHAPE_ID)->load());
            break;
        case OP1Params::SynthEngineType::Digital:
            digitalSynth.setParameter(0, apvts.getRawParameterValue(OP1Params::DIGI_WAVEFORM_ID)->load());
            digitalSynth.setParameter(1, apvts.getRawParameterValue(OP1Params::DIGI_DETUNE_ID)->load());
            digitalSynth.setParameter(2, apvts.getRawParameterValue(OP1Params::DIGI_PHASE_ID)->load());
            digitalSynth.setParameter(3, apvts.getRawParameterValue(OP1Params::DIGI_FOLD_ID)->load());
            break;
        case OP1Params::SynthEngineType::String:
            stringSynth.setParameter(0, apvts.getRawParameterValue(OP1Params::STR_DAMPING_ID)->load());
            stringSynth.setParameter(1, apvts.getRawParameterValue(OP1Params::STR_BODY_ID)->load());
            stringSynth.setParameter(2, apvts.getRawParameterValue(OP1Params::STR_PLUCK_ID)->load());
            stringSynth.setParameter(3, apvts.getRawParameterValue(OP1Params::STR_CHORUS_ID)->load());
            break;
        case OP1Params::SynthEngineType::Pulse:
            pulseSynth.setParameter(0, apvts.getRawParameterValue(OP1Params::PULSE_WIDTH_ID)->load());
            pulseSynth.setParameter(1, apvts.getRawParameterValue(OP1Params::PULSE_FINE_ID)->load());
            pulseSynth.setParameter(2, apvts.getRawParameterValue(OP1Params::PULSE_SHAPE_ID)->load());
            pulseSynth.setParameter(3, apvts.getRawParameterValue(OP1Params::PULSE_SYNC_ID)->load());
            break;
        case OP1Params::SynthEngineType::Cluster:
            clusterSynth.setParameter(0, apvts.getRawParameterValue(OP1Params::CLUST_SPREAD_ID)->load());
            clusterSynth.setParameter(1, apvts.getRawParameterValue(OP1Params::CLUST_VOICES_ID)->load() / 8.0f);
            clusterSynth.setParameter(2, apvts.getRawParameterValue(OP1Params::CLUST_DETUNE_ID)->load());
            clusterSynth.setParameter(3, apvts.getRawParameterValue(OP1Params::CLUST_SHAPE_ID)->load());
            break;
        case OP1Params::SynthEngineType::DSynth:
            dsynth.setParameter(0, apvts.getRawParameterValue(OP1Params::DSYN_ALGORITHM_ID)->load() / 7.0f);
            dsynth.setParameter(1, apvts.getRawParameterValue(OP1Params::DSYN_PARAM1_ID)->load());
            dsynth.setParameter(2, apvts.getRawParameterValue(OP1Params::DSYN_PARAM2_ID)->load());
            dsynth.setParameter(3, apvts.getRawParameterValue(OP1Params::DSYN_PARAM3_ID)->load());
            break;
        default:
            break;
    }

    // Mixer params
    masterVolume = apvts.getRawParameterValue(OP1Params::MIX_MASTER_VOL_ID)->load();
    masterDrive = apvts.getRawParameterValue(OP1Params::MIX_DRIVE_ID)->load();

    tapeRecorder.setTrackVolume(0, apvts.getRawParameterValue(OP1Params::MIX_TRACK1_VOL_ID)->load());
    tapeRecorder.setTrackVolume(1, apvts.getRawParameterValue(OP1Params::MIX_TRACK2_VOL_ID)->load());
    tapeRecorder.setTrackVolume(2, apvts.getRawParameterValue(OP1Params::MIX_TRACK3_VOL_ID)->load());
    tapeRecorder.setTrackVolume(3, apvts.getRawParameterValue(OP1Params::MIX_TRACK4_VOL_ID)->load());

    // Tape
    tapeRecorder.setSpeed(apvts.getRawParameterValue(OP1Params::TAPE_SPEED_ID)->load());
    tapeRecorder.setReverse(apvts.getRawParameterValue(OP1Params::TAPE_REVERSE_ID)->load() > 0.5f);
    tapeRecorder.setLoopIn(apvts.getRawParameterValue(OP1Params::TAPE_LOOP_IN_ID)->load());
    tapeRecorder.setLoopOut(apvts.getRawParameterValue(OP1Params::TAPE_LOOP_OUT_ID)->load());
    tapeRecorder.setActiveTrack((int)apvts.getRawParameterValue(OP1Params::TAPE_TRACK_ID)->load() - 1);

    // Tempo for sequencers
    float tempo = apvts.getRawParameterValue(OP1Params::TEMPO_ID)->load();
    endlessSeq.setTempo(tempo);
    patternSeq.setTempo(tempo);
    fingerSeq.setTempo(tempo);
    sketchSeq.setTempo(tempo);
}

SynthEngine* OP1FieldProcessor::getCurrentSynthEngine()
{
    switch (currentSynthType)
    {
        case OP1Params::SynthEngineType::FM:      return &fmSynth;
        case OP1Params::SynthEngineType::Digital:  return &digitalSynth;
        case OP1Params::SynthEngineType::String:   return &stringSynth;
        case OP1Params::SynthEngineType::Pulse:    return &pulseSynth;
        case OP1Params::SynthEngineType::Cluster:  return &clusterSynth;
        case OP1Params::SynthEngineType::DSynth:   return &dsynth;
        case OP1Params::SynthEngineType::Sampler:  return &samplerEngine;
    }
    return &fmSynth;
}

EffectProcessor* OP1FieldProcessor::getCurrentEffect()
{
    switch (currentEffectType)
    {
        case OP1Params::EffectType::Nitro:  return &nitroEffect;
        case OP1Params::EffectType::CWO:    return &cwoEffect;
        case OP1Params::EffectType::Delay:  return &delayEffect;
        case OP1Params::EffectType::Spring: return &springEffect;
        case OP1Params::EffectType::Phone:  return &phoneEffect;
        case OP1Params::EffectType::Punch:  return &punchEffect;
        case OP1Params::EffectType::None:   return nullptr;
    }
    return nullptr;
}

SequencerBase* OP1FieldProcessor::getCurrentSequencer()
{
    switch (currentSeqType)
    {
        case OP1Params::SequencerType::Endless: return &endlessSeq;
        case OP1Params::SequencerType::Pattern: return &patternSeq;
        case OP1Params::SequencerType::Finger:  return &fingerSeq;
        case OP1Params::SequencerType::Sketch:  return &sketchSeq;
        case OP1Params::SequencerType::None:    return nullptr;
    }
    return nullptr;
}

void OP1FieldProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    updateParametersFromAPVTS();

    // Process sequencer - generate MIDI events
    auto* sequencer = getCurrentSequencer();
    if (sequencer && sequencer->getPlaying())
    {
        juce::MidiBuffer seqMidi;
        sequencer->processBlock(seqMidi, buffer.getNumSamples());
        midiMessages.addEvents(seqMidi, 0, buffer.getNumSamples(), 0);
    }

    // Handle MIDI and synth rendering
    SynthEngine* engine = nullptr;
    if (currentMode == OP1Params::Mode::Synth)
        engine = getCurrentSynthEngine();
    else if (currentMode == OP1Params::Mode::Drum)
        engine = &drumSampler;

    if (engine)
    {
        for (const auto metadata : midiMessages)
        {
            auto msg = metadata.getMessage();
            if (msg.isNoteOn())
                engine->noteOn(msg.getNoteNumber(), msg.getFloatVelocity());
            else if (msg.isNoteOff())
                engine->noteOff(msg.getNoteNumber());
        }

        engine->renderBlock(buffer, 0, buffer.getNumSamples());
    }

    // Apply volume
    float synthVol = apvts.getRawParameterValue(OP1Params::SYNTH_VOLUME_ID)->load();
    buffer.applyGain(synthVol);

    // Apply effect
    auto* effect = getCurrentEffect();
    if (effect)
        effect->process(buffer);

    // If in tape mode, process tape
    if (currentMode == OP1Params::Mode::Tape)
    {
        juce::AudioBuffer<float> tapeOutput(buffer.getNumChannels(), buffer.getNumSamples());
        tapeRecorder.process(buffer, tapeOutput);

        // Mix tape output with live signal
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            buffer.addFrom(ch, 0, tapeOutput, ch, 0, buffer.getNumSamples());
    }

    // Master drive (saturation)
    if (masterDrive > 0.01f)
    {
        float driveGain = 1.0f + masterDrive * 5.0f;
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < buffer.getNumSamples(); ++i)
                data[i] = std::tanh(data[i] * driveGain);
        }
    }

    // Master volume
    buffer.applyGain(masterVolume);

    // Update level metering
    float peakL = 0.0f, peakR = 0.0f;
    if (buffer.getNumChannels() >= 1)
        peakL = buffer.getMagnitude(0, 0, buffer.getNumSamples());
    if (buffer.getNumChannels() >= 2)
        peakR = buffer.getMagnitude(1, 0, buffer.getNumSamples());
    outputLevelL.store(peakL);
    outputLevelR.store(peakR);
}

juce::AudioProcessorEditor* OP1FieldProcessor::createEditor()
{
    return new OP1FieldEditor(*this);
}

void OP1FieldProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void OP1FieldProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState && xmlState->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

// Plugin factory
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OP1FieldProcessor();
}
