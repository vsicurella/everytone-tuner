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


    // Load 31 edo with a C center in channel 4, periodic mapping
    auto def31edo = Tuning::CentsDefinition(31);
    def31edo.reference.rootMidiChannel = 4;
    def31edo.reference.rootMidiNote = 60;
    def31edo.reference.rootFrequency = 261.6255653;

    auto ode31 = Tuning(def31edo);
    audioProcessor.loadTuningTarget(ode31);

    auto period31On4C60 = Keytographer::MultichannelMap::CreatePeriodicMapping(31, 60, 4);
    audioProcessor.loadNoteMapping(period31On4C60);
    
    mainWindow->setTuningDisplayed(ode31);
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
