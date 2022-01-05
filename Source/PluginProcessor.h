/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "TuningChanger.h"
#include "TunerController.h"
#include "MidiVoiceController.h"
#include "MidiVoiceInterpolator.h"

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

    const MappedTuningTable* currentSource() const { return tunerController->readTuningSource(); }

    const MappedTuningTable* currentTarget() const { return tunerController->readTuningTarget(); }

    //==============================================================================

    void addTunerControllerWatcher(TunerController::Watcher* watcher) { tunerController->addWatcher(watcher); }
    void removeTunerControllerWatcher(TunerController::Watcher* watcher) { tunerController->removeWatcher(watcher); }

    void loadTuningSource(const CentsDefinition& tuningDefinition);
    void loadTuningTarget(const CentsDefinition& tuningDefinition);

    void setTuningSource(std::shared_ptr<TuningTable> sourceTuning);
    void setTuningTarget(std::shared_ptr<TuningTable> targetTuning);

    void setTargetTuningReference(MappedTuningTable::FrequencyReference reference);
    void setTargetTuningRootFrequency(double frequency);
    void setTargetMappingRoot(TuningTableMap::Root root);
    void setTargetMappedTuningRoot(MappedTuningTable::Root root);

    //void loadNoteMapping(const TuningTableMap& map);

    //==============================================================================

    Everytone::Options options() const;
    void options(Everytone::Options optionsIn);

    Everytone::MappingType autoMappingType() const { return tunerController->getMappingType(); }
    void autoMappingType(Everytone::MappingType type);

    Everytone::MappingMode mappingMode() const { return tunerController->getMappingMode(); }
    void mappingMode(Everytone::MappingMode mode);

    Everytone::ChannelMode channelMode() const { return voiceController->getChannelMode(); }
    void channelMode(Everytone::ChannelMode mode);

    Everytone::MpeZone mpeZone() const { return voiceController->getMpeZone(); }
    void mpeZone(Everytone::MpeZone zone);

    int voiceLimit() const { return voiceController->getVoiceLimit(); }
    void voiceLimit(int voiceLimit);

    int pitchbendRange() const { return tunerController->getPitchbendRange(); }
    void pitchbendRange(int pitchbendRange);

    Everytone::BendMode bendMode() const { return voiceInterpolator->getBendMode(); }
    void bendMode(Everytone::BendMode bendMode);

    //==============================================================================


private:

    void tuneMidiBuffer(juce::MidiBuffer& buffer);

    //void testMidi();

private:

    std::unique_ptr<TunerController> tunerController;
    std::unique_ptr<MidiVoiceController> voiceController;
    std::unique_ptr<MidiVoiceInterpolator> voiceInterpolator;
    
    std::unique_ptr<MultimapperLog> logger;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultimapperAudioProcessor)
};
