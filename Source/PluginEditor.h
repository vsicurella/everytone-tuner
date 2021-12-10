/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LogWindow.h"
#include "UI/MainWindow.h"
#include "UI/MenuBar.h"
#include "UI/NewTuningPanel.h"
#include "../Source/MultichannelMap.h"



//==============================================================================
/**
*/
class MultimapperAudioProcessorEditor  : public juce::AudioProcessorEditor, 
                                         public TuningWatcher, 
                                         public juce::ApplicationCommandManager,
                                         public juce::ApplicationCommandTarget
{
public:
    MultimapperAudioProcessorEditor (MultimapperAudioProcessor&);
    ~MultimapperAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    //==============================================================================
    // TuningWatcher implementation

    void tuningChanged(TuningChanger* changer, Tuning* tuning) override;

    //==============================================================================
    // ApplicationCommandManager implementation
    virtual ApplicationCommandTarget* getFirstCommandTarget(juce::CommandID commandID) override;

    //==============================================================================
    // ApplicationCommandTarget implementation

    ApplicationCommandTarget* getNextCommandTarget() override;
    void getAllCommands(juce::Array<juce::CommandID>& commands) override;
    void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result) override;
    bool perform(const InvocationInfo& info) override;

    //==============================================================================


    void setContentComponent(juce::Component* component);

private:

    void setupCommands();

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MultimapperAudioProcessor& audioProcessor;

    MenuBarModel menuModel;

    std::unique_ptr<juce::MenuBarComponent> menuBar;

    juce::Component* contentComponent = nullptr;
    std::unique_ptr<MainWindow> mainWindow;
    std::unique_ptr<NewTuningPanel> newTuningPanel;

    std::unique_ptr<LogWindow> logger;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultimapperAudioProcessorEditor)
};
