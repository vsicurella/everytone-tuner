/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MultimapperAudioProcessorEditor::MultimapperAudioProcessorEditor (MultimapperAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), menuModel(static_cast<juce::ApplicationCommandManager*>(this))
{

    menuBar.reset(new juce::MenuBarComponent(&menuModel));
    addAndMakeVisible(*menuBar);

    mainWindow.reset(new MainWindow());
    addAndMakeVisible(mainWindow.get());
    contentComponent = mainWindow.get();

    setSize (600, 250);

#if JUCE_DEBUG
    logger.reset(new LogWindow([&]() -> juce::StringRef { return audioProcessor.getLog(); }));
    logger->setSize(800, 600);
    logger->setVisible(true);
#endif

    setupCommands();

    // Load 31 edo with a C center in channel 4, periodic mapping
    //auto def31edo = Tuning::CentsDefinition(31);
    //def31edo.reference.rootMidiChannel = 4;
    //def31edo.reference.rootMidiNote = 60;
    //def31edo.reference.rootFrequency = 261.6255653;

    //auto ode31 = Tuning(def31edo);
    //audioProcessor.loadTuningTarget(ode31);

    //auto period31On4C60 = Keytographer::MultichannelMap::CreatePeriodicMapping(31, 0, 4);
    //audioProcessor.loadNoteMapping(period31On4C60);
    //
    //mainWindow->setTuningDisplayed(ode31);
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
    
    //mainWindow->setBounds(getLocalBounds().withTop(menuHeight).withTrimmedBottom(menuHeight));
    if (contentComponent != nullptr)
    {
        contentComponent->setBounds(getLocalBounds().withTop(menuHeight).withTrimmedBottom(menuHeight));
    }

}

void MultimapperAudioProcessorEditor::tuningChanged(TuningChanger* changer, Tuning* tuning)
{
    audioProcessor.loadTuningTarget(*tuning);
}

juce::ApplicationCommandTarget* MultimapperAudioProcessorEditor::getFirstCommandTarget(juce::CommandID commandID)
{
    switch (commandID)
    {
    case Multimapper::Commands::Back:
    case Multimapper::Commands::NewTuning:
        return this;

    default:
        break;
    }

    return nullptr;
}

juce::ApplicationCommandTarget* MultimapperAudioProcessorEditor::getNextCommandTarget()
{
    return nullptr;
}

void MultimapperAudioProcessorEditor::getAllCommands(juce::Array<juce::CommandID>& commands)
{
    commands =
    {
        Multimapper::Back,
        Multimapper::NewTuning,
    };
}

void MultimapperAudioProcessorEditor::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result)
{
    switch (commandID)
    {
    case Multimapper::Back:
        result = juce::ApplicationCommandInfo(Multimapper::Commands::Back);
        result.setInfo("Back", "Back to main window", "", 0);
        result.addDefaultKeypress(juce::KeyPress::escapeKey, juce::ModifierKeys::noModifiers);
        break;

    case Multimapper::NewTuning:
        result = juce::ApplicationCommandInfo(Multimapper::Commands::NewTuning);
        result.setInfo("New", "Create new tuning", "Scale", 0);
        result.addDefaultKeypress('n', juce::ModifierKeys::ctrlModifier);
        break;

    default:
        // Forgot to add commandInfo?
        jassertfalse;
    }
}

bool MultimapperAudioProcessorEditor::perform(const InvocationInfo& info)
{
    switch (info.commandID)
    {
    case Multimapper::Back:
        setContentComponent(mainWindow.get());
        return true;

    case Multimapper::NewTuning:
        if (newTuningPanel == nullptr)
        {
            newTuningPanel.reset(new NewTuningPanel(this));
            addChildComponent(*newTuningPanel);
            newTuningPanel->addTuningWatcher(mainWindow.get());
        }
        setContentComponent(newTuningPanel.get());
        return true;

    default:
        // forgot to add command handler?
        jassertfalse;
    }

    return false;
}

void MultimapperAudioProcessorEditor::setContentComponent(juce::Component* component)
{
    if (contentComponent != nullptr)
        contentComponent->setVisible(false);

    contentComponent = component;
    contentComponent->setVisible(true);
    resized();
}

void MultimapperAudioProcessorEditor::setupCommands()
{
    registerAllCommandsForTarget(this);
}