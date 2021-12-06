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



//==============================================================================
/**
*/
class MultimapperAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    MultimapperAudioProcessorEditor (MultimapperAudioProcessor&);
    ~MultimapperAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MultimapperAudioProcessor& audioProcessor;

    MenuBarModel menuModel;

    std::unique_ptr<juce::MenuBarComponent> menuBar;
    std::unique_ptr<MainWindow> mainWindow;

    std::unique_ptr<LogWindow> logger;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultimapperAudioProcessorEditor)
};
