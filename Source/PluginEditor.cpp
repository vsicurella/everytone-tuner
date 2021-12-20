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
    tuningBackup.reset(new Tuning(*audioProcessor.activeTargetTuning()));

    menuBar.reset(new juce::MenuBarComponent(&menuModel));
    addAndMakeVisible(*menuBar);

    overviewPanel.reset(new OverviewPanel(audioProcessor.options()));
    addAndMakeVisible(overviewPanel.get());
    overviewPanel->setTuningDisplayed(tuningBackup.get());
    //overviewPanel->addMappingWatcher(this);
    overviewPanel->addOptionsWatcher(this);
    overviewPanel->addTuningWatcher(this);

    contentComponent = overviewPanel.get();

    audioProcessor.addTuningWatcher(this);

    setSize (600, 250);

#if JUCE_DEBUG
    logger.reset(new LogWindow([&]() -> juce::StringRef { return audioProcessor.getLog(); }));
    logger->setSize(800, 600);
    logger->setVisible(true);
#endif

    setupCommands();

    getKeyMappings()->addKeyPress(Multimapper::Commands::Back, juce::KeyPress::createFromDescription("ctrl + -"));

    addKeyListener(getKeyMappings());
}

MultimapperAudioProcessorEditor::~MultimapperAudioProcessorEditor()
{
    logger = nullptr;
    overviewPanel = nullptr;
}

//==============================================================================
void MultimapperAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MultimapperAudioProcessorEditor::resized()
{
    double menuHeight = getHeight() * 0.11;
    menuBar->setBounds(getLocalBounds().withBottom(menuHeight));
    
    int margin = 5;

    //overviewPanel->setBounds(getLocalBounds().withTop(menuHeight).withTrimmedBottom(menuHeight));
    if (contentComponent != nullptr)
    {
        contentComponent->setBounds(getLocalBounds().withTop(menuHeight).reduced(margin));
    }

}

void MultimapperAudioProcessorEditor::tuningTargetChanged(TuningChanger* changer, const Tuning* tuning)
{
    if (changer == &audioProcessor)
        overviewPanel->setTuningDisplayed(tuning);
    else
        audioProcessor.loadTuningTarget(*tuning);
}

void MultimapperAudioProcessorEditor::tuningTargetReferenceChanged(TuningChanger* changer, Tuning::Reference reference)
{
    if (changer == &audioProcessor)
        overviewPanel->setTuningDisplayed(audioProcessor.activeTargetTuning());
    else
        audioProcessor.setTargetTuningReference(reference);
}

void MultimapperAudioProcessorEditor::mappingModeChanged(Multimapper::MappingMode mode)
{
    audioProcessor.setMappingMode(mode);
}

void MultimapperAudioProcessorEditor::mappingTypeChanged(Multimapper::MappingType type)
{
    audioProcessor.setAutoMappingType(type);
}

void MultimapperAudioProcessorEditor::channelModeChanged(Multimapper::ChannelMode newChannelMode)
{
    audioProcessor.setChannelMode(newChannelMode);
}

void MultimapperAudioProcessorEditor::midiModeChanged(Multimapper::MidiMode newMidiMode)
{
    
}

void MultimapperAudioProcessorEditor::voiceLimitChanged(int newVoiceLimit)
{
    audioProcessor.setVoiceLimit(newVoiceLimit);
}


juce::ApplicationCommandTarget* MultimapperAudioProcessorEditor::getFirstCommandTarget(juce::CommandID commandID)
{
    switch (commandID)
    {
    default:
        return this;
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
        Multimapper::Save,
        Multimapper::NewTuning,
        Multimapper::OpenTuning,
        Multimapper::ShowOptions
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
        result.setActive(contentComponent != overviewPanel.get());
        break;

    case Multimapper::Save:
        result = juce::ApplicationCommandInfo(Multimapper::Commands::Save);
        result.setInfo("Save", "Save currently edited tuning", "", 0);
        result.addDefaultKeypress('s', juce::ModifierKeys::ctrlModifier);
        break;

    case Multimapper::NewTuning:
        result = juce::ApplicationCommandInfo(Multimapper::Commands::NewTuning);
        result.setInfo("New Tuning", "Create new tuning", "Scale", 0);
        result.addDefaultKeypress('n', juce::ModifierKeys::ctrlModifier);
        break;

    case Multimapper::OpenTuning:
        result = juce::ApplicationCommandInfo(Multimapper::Commands::OpenTuning);
        result.setInfo("Open Tuning", "Load a .scl or .tun file", "Scale", 0);
        result.addDefaultKeypress('o', juce::ModifierKeys::ctrlModifier);
        break;

    case Multimapper::ShowOptions:
        result = juce::ApplicationCommandInfo(Multimapper::Commands::ShowOptions);
        result.setInfo("Show Options", "Change some advanced midi tuning options", "Options", 0);
        result.addDefaultKeypress('p', juce::ModifierKeys::ctrlModifier);
        break;

    default:
        // Forgot to add commandInfo?
        jassertfalse;
    }
}

bool MultimapperAudioProcessorEditor::perform(const juce::ApplicationCommandTarget::InvocationInfo& info)
{
    switch (info.commandID)
    {
    case Multimapper::Back:
        return performBack(info);

    case Multimapper::Save:
        return performSave(info);

    case Multimapper::NewTuning:
        return performNewTuning(info);
    
    case Multimapper::OpenTuning:
        return performOpenTuning(info);

    case Multimapper::ShowOptions:
        return performShowOptions(info);

    default:
        // forgot to add command handler?
        jassertfalse;
    }

    return false;
}

bool MultimapperAudioProcessorEditor::performBack(const juce::ApplicationCommandTarget::InvocationInfo& info)
{
    if (contentComponent == overviewPanel.get())
        return false;

    if (contentComponent == newTuningPanel.get())
    {
        // Revert tuning if preview is on
        if (newTuningPanel->previewOn())
        {
            audioProcessor.loadTuningTarget(*tuningBackup);
        }
    }

    setContentComponent(overviewPanel.get());
    return true;
}

bool MultimapperAudioProcessorEditor::performSave(const juce::ApplicationCommandTarget::InvocationInfo& info)
{
    if (contentComponent == newTuningPanel.get())
    {
        // Load tuning if it wasn't previewed
        if (!newTuningPanel->previewOn())
        {
            commitTuning(newTuningPanel->stagedTuning());
        }
    }

    setContentComponent(overviewPanel.get());

    return true;
}

bool MultimapperAudioProcessorEditor::performNewTuning(const juce::ApplicationCommandTarget::InvocationInfo& info)
{
    if (newTuningPanel == nullptr)
    {
        newTuningPanel.reset(new NewTuningPanel(this));
        addChildComponent(*newTuningPanel);
    }
    
    setContentComponent(newTuningPanel.get());

    return true;
}

bool MultimapperAudioProcessorEditor::performOpenTuning(const juce::ApplicationCommandTarget::InvocationInfo& info)
{
    fileChooser = std::make_unique<juce::FileChooser>("Choose a .scl or .tun file", juce::File() /* TODO */, "*.scl;*.tun");
    fileChooser->launchAsync(
        juce::FileBrowserComponent::FileChooserFlags::openMode | juce::FileBrowserComponent::FileChooserFlags::canSelectFiles,
        [&](const juce::FileChooser& chooser)
        {
            auto result = chooser.getResult();
            if (result.existsAsFile())
            {
                TuningFileParser parser(result);
                commitTuning(parser.getTuning());
            }
        });

    return true;
}

bool MultimapperAudioProcessorEditor::performShowOptions(const juce::ApplicationCommandTarget::InvocationInfo& info)
{
    if (optionsPanel == nullptr)
    {
        optionsPanel = std::make_unique<OptionsPanel>(audioProcessor.options());
        optionsPanel->addOptionsWatcher(this);
        addChildComponent(*optionsPanel);
    }

    setContentComponent(optionsPanel.get());
    return true;
}


void MultimapperAudioProcessorEditor::commitTuning(const Tuning* tuning)
{
    audioProcessor.loadTuningTarget(*tuning);
    auto savedTuning = audioProcessor.activeTargetTuning();
    tuningBackup.reset(new Tuning(*savedTuning));
    overviewPanel->setTuningDisplayed(savedTuning);
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