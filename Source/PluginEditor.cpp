/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MultimapperAudioProcessorEditor::MultimapperAudioProcessorEditor (MultimapperAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    menuBar.reset(new juce::MenuBarComponent(&menuModel));
    addAndMakeVisible(*menuBar);

    mainWindow.reset(new MainWindow());
    addAndMakeVisible(*mainWindow);

    setSize (600, 250);

#if JUCE_DEBUG
    logger.reset(new LogWindow([&]() -> juce::StringRef { return audioProcessor.getLog(); }));
    logger->setSize(800, 600);
    logger->setVisible(true);
#endif

}

MultimapperAudioProcessorEditor::~MultimapperAudioProcessorEditor()
{
    logger = nullptr;
    mainWindow = nullptr;
}

//==============================================================================
void MultimapperAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MultimapperAudioProcessorEditor::resized()
{
    double menuHeight = getHeight() * 0.125;

    menuBar->setBounds(getLocalBounds().withBottom(menuHeight));
    mainWindow->setBounds(getLocalBounds().withTop(menuHeight).withTrimmedBottom(menuHeight));
}
