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
    tuningBackup = std::make_unique<MappedTuning>(*audioProcessor.currentTarget());

    menuBar.reset(new juce::MenuBarComponent(&menuModel));
    addAndMakeVisible(*menuBar);

    overviewPanel.reset(new OverviewPanel(audioProcessor.options()));
    overviewPanel->setTuningDisplayed(audioProcessor.currentTarget());
    addAndMakeVisible(overviewPanel.get());
    overviewPanel->addOptionsWatcher(this);
    
    overviewPanel->addMappingWatcher(this);
    audioProcessor.addTunerControllerWatcher(this);

    contentComponent = overviewPanel.get();

    setSize (600, 250);

#if JUCE_DEBUG
    auto logger = audioProcessor.getLog();
    logWindow.reset(new LogWindow(logger->getAllMessages()));
    logWindow->setSize(800, 600);
    logWindow->setVisible(true);
    logger->setCallback([&](juce::StringRef msg) { logWindow->addMessage(msg); });
#endif

    setupCommands();

    getKeyMappings()->addKeyPress(Everytone::Commands::Back, juce::KeyPress::createFromDescription("ctrl + -"));

    addKeyListener(getKeyMappings());
}

MultimapperAudioProcessorEditor::~MultimapperAudioProcessorEditor()
{
#if JUCE_DEBUG
    auto logger = audioProcessor.getLog();
    logger->setCallback([](juce::StringRef) {});
#endif

    newTuningPanel = nullptr;
    overviewPanel = nullptr;
    logWindow = nullptr;

    audioProcessor.removeTunerControllerWatcher(this);
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

void MultimapperAudioProcessorEditor::sourceTuningChanged(const MappedTuning& source)
{

}

void MultimapperAudioProcessorEditor::targetTuningChanged(const MappedTuning& target)
{
    overviewPanel->setTuningDisplayed(&target);
}


void MultimapperAudioProcessorEditor::targetDefinitionLoaded(TuningChanger* changer, CentsDefinition definition)
{
    audioProcessor.loadTuningTarget(definition);
}

void MultimapperAudioProcessorEditor::targetMappedTuningLoaded(TuningChanger* changer, CentsDefinition tuningDefinition, TuningTableMap::Definition mapDefinition)
{
    
}

void MultimapperAudioProcessorEditor::targetRootFrequencyChanged(TuningChanger* changer, double frequency)
{
    audioProcessor.setTargetTuningRootFrequency(frequency);
}

void MultimapperAudioProcessorEditor::mappingTypeChanged(Everytone::MappingType type)
{
    audioProcessor.setAutoMappingType(type);
}

void MultimapperAudioProcessorEditor::mappingRootChanged(int rootMidiChannel, int rootMidiNote)
{
    audioProcessor.setTargetMappingRoot(rootMidiChannel, rootMidiNote);
}

void MultimapperAudioProcessorEditor::mappingModeChanged(Everytone::MappingMode mode)
{
    audioProcessor.setMappingMode(mode);
}

void MultimapperAudioProcessorEditor::channelModeChanged(Everytone::ChannelMode newChannelMode)
{
    audioProcessor.setChannelMode(newChannelMode);
}

void MultimapperAudioProcessorEditor::mpeZoneChanged(Everytone::MpeZone zone)
{
    audioProcessor.setMpeZone(zone);
}

void MultimapperAudioProcessorEditor::midiModeChanged(Everytone::MidiMode newMidiMode)
{
    
}

void MultimapperAudioProcessorEditor::voiceLimitChanged(int newVoiceLimit)
{
    audioProcessor.setVoiceLimit(newVoiceLimit);
}

void MultimapperAudioProcessorEditor::pitchbendRangeChanged(int pitchbendRange)
{
    // User inputs bipolar form, so multiply for absolute range
    audioProcessor.setPitchbendRange(pitchbendRange * 2);
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
        Everytone::Back,
        Everytone::Save,
        Everytone::NewTuning,
        Everytone::OpenTuning,
        Everytone::ShowOptions
    };
}

void MultimapperAudioProcessorEditor::getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo& result)
{
    switch (commandID)
    {
    case Everytone::Back:
        result = juce::ApplicationCommandInfo(Everytone::Commands::Back);
        result.setInfo("Back", "Back to main window", "", 0);
        result.addDefaultKeypress(juce::KeyPress::escapeKey, juce::ModifierKeys::noModifiers);
        result.setActive(contentComponent != overviewPanel.get());
        break;

    case Everytone::Save:
        result = juce::ApplicationCommandInfo(Everytone::Commands::Save);
        result.setInfo("Save", "Save currently edited tuning", "", 0);
        result.addDefaultKeypress('s', juce::ModifierKeys::ctrlModifier);
        break;

    case Everytone::NewTuning:
        result = juce::ApplicationCommandInfo(Everytone::Commands::NewTuning);
        result.setInfo("New Tuning", "Create new tuning", "Scale", 0);
        result.addDefaultKeypress('n', juce::ModifierKeys::ctrlModifier);
        break;

    case Everytone::OpenTuning:
        result = juce::ApplicationCommandInfo(Everytone::Commands::OpenTuning);
        result.setInfo("Open Tuning", "Load a .scl or .tun file", "Scale", 0);
        result.addDefaultKeypress('o', juce::ModifierKeys::ctrlModifier);
        break;

    case Everytone::ShowOptions:
        result = juce::ApplicationCommandInfo(Everytone::Commands::ShowOptions);
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
    case Everytone::Back:
        return performBack(info);

    case Everytone::Save:
        return performSave(info);

    case Everytone::NewTuning:
        return performNewTuning(info);
    
    case Everytone::OpenTuning:
        return performOpenTuning(info);

    case Everytone::ShowOptions:
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
        // TODO: finish preview implementation
        // Revert tuning if preview is on
        //if (newTuningPanel->previewOn())
        //{
        //    auto definition
        //    audioProcessor.loadTuningTarget(*tuningBackup);
        //}
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
                commitTuning(parser.getTuningDefinition());
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

void MultimapperAudioProcessorEditor::commitTuning(CentsDefinition tuningDefinition)
{
    audioProcessor.loadTuningTarget(tuningDefinition);
    tuningBackup = std::make_unique<MappedTuning>(*audioProcessor.currentTarget());
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