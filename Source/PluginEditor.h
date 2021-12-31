/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "PluginProcessor.h"
#include "LogWindow.h"
#include "ui/InfoBar.h"
#include "ui/MenuPanel.h"
#include "ui/OverviewPanel.h"
#include "ui/NewTuningPanel.h"
#include "ui/MappingPanel.h"
#include "ui/OptionsPanel.h"
#include "io/TuningFileParser.h"

//==============================================================================
/**
*/
class MultimapperAudioProcessorEditor  : public juce::AudioProcessorEditor, 
                                         public juce::ApplicationCommandManager,
                                         public juce::ApplicationCommandTarget,
                                         public TunerController::Watcher,
                                         public TuningWatcher,
                                         public OptionsWatcher
{
public:
    MultimapperAudioProcessorEditor (MultimapperAudioProcessor&);
    ~MultimapperAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    //==============================================================================
    // TunerController::Watcher implementation

    void sourceTuningChanged(const std::shared_ptr<MappedTuningTable>& source) override;

    void targetTuningChanged(const std::shared_ptr<MappedTuningTable>& target) override;


    //==============================================================================
    // TuningWatcher implementation

    void targetDefinitionLoaded(TuningChanger* changer, CentsDefinition definition) override;

    void targetMappedTuningLoaded(TuningChanger* changer, CentsDefinition tuningDefinition, TuningTableMap::Definition mapDefinition) override;

    void targetRootFrequencyChanged(TuningChanger* changer, double frequency) override;

    void targetMappedTuningRootChanged(TuningChanger* changer, MappedTuningTable::Root root) override;

    //==============================================================================
    // OptionsWatcher implementation

    void mappingModeChanged(Everytone::MappingMode mode) override;
    void mappingTypeChanged(Everytone::MappingType type) override;
    void channelModeChanged(Everytone::ChannelMode newChannelMode) override;
    void mpeZoneChanged(Everytone::MpeZone zone) override;
    void midiModeChanged(Everytone::MidiMode newMidiMode) override;
    void voiceLimitChanged(int newVoiceLimit) override;
    void pitchbendRangeChanged(int pitchbendRange) override;
    void bendModeChanged(Everytone::BendMode newBendMode) override;

    //==============================================================================
    // ApplicationCommandManager implementation
    virtual ApplicationCommandTarget* getFirstCommandTarget(juce::CommandID commandID) override;

    //==============================================================================
    // ApplicationCommandTarget implementation

    ApplicationCommandTarget* getNextCommandTarget() override;
    void getAllCommands(juce::Array<juce::CommandID>& commands) override;
    void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) override;
    bool perform(const juce::ApplicationCommandTarget::InvocationInfo& info) override;

    //==============================================================================
    // ApplicationCommand perfom callbacks

    bool performBack(const juce::ApplicationCommandTarget::InvocationInfo& info);

    bool performSave(const juce::ApplicationCommandTarget::InvocationInfo& info);

    bool performOpenTuning(const juce::ApplicationCommandTarget::InvocationInfo& info);

    //==============================================================================

    void commitTuning(CentsDefinition tuningDefinition);

    void setContentComponent(juce::Component* component);

private:

    void setupCommands();

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MultimapperAudioProcessor& audioProcessor;

    std::unique_ptr<MappedTuningTable> tuningBackup;

    juce::Component* contentComponent = nullptr;

    std::unique_ptr<InfoBar> infoBar;
    std::unique_ptr<MenuPanel> menuPanel;

    //std::unique_ptr<IntervalListModel> intervalListModel;

    std::unique_ptr<OverviewPanel> overviewPanel;
    std::unique_ptr<NewTuningPanel> newTuningPanel;
    std::unique_ptr<MappingPanel> mappingPanel;
    std::unique_ptr<OptionsPanel> optionsPanel;


    std::unique_ptr<juce::FileChooser> fileChooser;

    std::unique_ptr<LogWindow> logWindow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultimapperAudioProcessorEditor)
};
