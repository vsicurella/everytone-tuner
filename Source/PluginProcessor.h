/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "TuningChanger.h"
#include "MidiBrain.h"
#include "MappedTuningController.h"

#if RUN_MULTIMAPPER_TESTS
    #include "./tests/Tuning_tests.h"
#endif

class MultimapperLog : public juce::Logger
{
    std::function<void(juce::StringRef msg)> callback;

public:
    MultimapperLog(std::function<void(juce::StringRef msg)> logCallback)
        : callback(logCallback) {}

    void logMessage(const juce::String& msg) override
    {
        DBG(msg);
        callback(msg);
    }
};

//==============================================================================
/**
*/
class MultimapperAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    MultimapperAudioProcessor();
    ~MultimapperAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::String getLog() const;

    //==============================================================================

    const Tuning* activeSourceTuning() const;
    const Tuning* activeTargetTuning() const;

    //==============================================================================

    void loadTuningSource(const Tuning& tuning);
    void loadTuningTarget(const Tuning& tuning);

    void setTargetTuningReference(Tuning::Reference reference);

    void loadNoteMapping(const Keytographer::TuningTableMap& map);

    void setAutoMappingType(Multimapper::MappingType type);

    void refreshAutoMapping();

private:

    void testMidi();

private:

    std::unique_ptr<Tuning> tuningSource;
    std::unique_ptr<Tuning> tuningTarget;

    std::unique_ptr<Keytographer::TuningTableMap> noteMap;

    MappedTuningController tuningMapHelper;

    Multimapper::MappingMode mappingMode = Multimapper::MappingMode::Auto;
    Multimapper::MappingType mappingType = Multimapper::MappingType::Linear;

    MidiVoiceController voiceController;
    
    std::unique_ptr<MidiBrain> midiBrain;
    
    juce::String dbgLog;
    std::unique_ptr<MultimapperLog> logger;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultimapperAudioProcessor)
};
