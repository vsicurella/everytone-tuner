/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LogWindow.h"
#include "ui/OverviewPanel.h"
#include "ui/MenuBar.h"
#include "ui/NewTuningPanel.h"
#include "ui/OptionsPanel.h"
#include "io/TuningFileParser.h"
#include "./mapping/MultichannelMap.h"

//==============================================================================
/**
*/
class MultimapperAudioProcessorEditor  : public juce::AudioProcessorEditor, 
                                         public juce::ApplicationCommandManager,
                                         public juce::ApplicationCommandTarget,
                                         public TuningWatcher,
                                         public TuningChanger,
                                         public OptionsWatcher
{
public:
    MultimapperAudioProcessorEditor (MultimapperAudioProcessor&);
    ~MultimapperAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    //==============================================================================
    // TuningWatcher implementation

    void tuningTargetChanged(TuningChanger* changer, const Tuning* tuning) override;

    void tuningTargetReferenceChanged(TuningChanger* changer, Tuning::Reference reference) override;

    //==============================================================================
    // MappingWatcher implementation

    //void mappingTypeChanged(MappingChanger* changer, Everytone::MappingType type) override;

    //==============================================================================
    // OptionsWatcher implementation

    void mappingModeChanged(Everytone::MappingMode mode) override;
    void mappingTypeChanged(Everytone::MappingType type) override;
    void channelModeChanged(Everytone::ChannelMode newChannelMode) override;
    void midiModeChanged(Everytone::MidiMode newMidiMode) override;
    void voiceLimitChanged(int newVoiceLimit) override;
    void pitchbendRangeChanged(int pitchbendRange) override;

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

    bool performNewTuning(const juce::ApplicationCommandTarget::InvocationInfo& info);

    bool performOpenTuning(const juce::ApplicationCommandTarget::InvocationInfo& info);

    bool performShowOptions(const juce::ApplicationCommandTarget::InvocationInfo& info);

    //==============================================================================

    void commitTuning(const Tuning* tuning);

    void setContentComponent(juce::Component* component);

private:

    void setupCommands();

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MultimapperAudioProcessor& audioProcessor;

    std::unique_ptr<Tuning> tuningBackup;

    MenuBarModel menuModel;

    std::unique_ptr<juce::MenuBarComponent> menuBar;

    juce::Component* contentComponent = nullptr;
    std::unique_ptr<OverviewPanel> overviewPanel;
    std::unique_ptr<NewTuningPanel> newTuningPanel;
    std::unique_ptr<OptionsPanel> optionsPanel;

    std::unique_ptr<juce::FileChooser> fileChooser;

    std::unique_ptr<LogWindow> logger;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultimapperAudioProcessorEditor)
};
