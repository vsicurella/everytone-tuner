/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "TuningChanger.h"
#include "MappedTuningController.h"
#include "MidiVoiceController.h"

class MultimapperLog : public juce::Logger
{
    juce::StringArray log;
    std::function<void(juce::StringRef msg)> callback = [](juce::StringRef) {};

public:
    MultimapperLog() {}

    void logMessage(const juce::String& msg) override
    {
        DBG(msg);
        log.add(msg);
        callback(msg);
    }

    juce::StringArray getAllMessages() const { return log; }

    void setCallback(std::function<void(juce::StringRef)> callbackIn) { callback = callbackIn; }
};

//==============================================================================
/**
*/
class MultimapperAudioProcessor  : public juce::AudioProcessor, public TuningChanger
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

    MultimapperLog* getLog() const;

    //==============================================================================

    const Tuning* activeSourceTuning() const { return tuningController.readTuningSource(); }

    const Tuning* activeTargetTuning() const { return tuningController.readTuningTarget(); }

    const TuningTableMap* noteMapping() const { return tuningController.readMapping(); }

    Everytone::Options options() const;

    //==============================================================================

    void loadTuningSource(const Tuning& tuning);
    void loadTuningTarget(const Tuning& tuning);

    void setTargetTuningReference(Tuning::Reference reference);

    void loadNoteMapping(const TuningTableMap& map);

    //==============================================================================

    void setAutoMappingType(Everytone::MappingType type);

    void setMappingMode(Everytone::MappingMode mode);

    void setChannelMode(Everytone::ChannelMode mode);

    void setMpeZone(Everytone::MpeZone zone);

    void setVoiceLimit(int voiceLimit);

    void setPitchbendRange(int pitchbendRange);

    void setOptions(Everytone::Options optionsIn);

    //==============================================================================


private:

    void tuneMidiBuffer(juce::MidiBuffer& buffer);


    void testMidi();

private:

    MappedTuningController tuningController;
    MidiVoiceController voiceController;
    
    std::unique_ptr<MultimapperLog> logger;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultimapperAudioProcessor)
};
