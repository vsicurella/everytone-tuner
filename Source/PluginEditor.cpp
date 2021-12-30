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
    tuningBackup = std::make_unique<MappedTuningTable>(*audioProcessor.currentTarget());

    auto currentTarget = audioProcessor.currentTarget();

    infoBar = std::make_unique<InfoBar>(this);
    infoBar->setDisplayedTuning(currentTarget);
    addAndMakeVisible(*infoBar);

    menuPanel = std::make_unique<MenuPanel>(this);
    addChildComponent(*menuPanel);

    intervalListModel = std::make_unique<IntervalListModel>(false, currentTarget);
    intervalListModel->addTuningWatcher(this);

    overviewPanel = std::make_unique<OverviewPanel>(audioProcessor.options());
    overviewPanel->setListEditorModel(intervalListModel.get());
    overviewPanel->setTuningDisplayed(currentTarget);
    overviewPanel->addTuningWatcher(this);
    overviewPanel->addOptionsWatcher(this);
    addAndMakeVisible(*overviewPanel);

    newTuningPanel = std::make_unique<NewTuningPanel>(this);
    newTuningPanel->addTuningWatcher(this);
    addChildComponent(*newTuningPanel);

    mappingPanel = std::make_unique<MappingPanel>(audioProcessor.options());
    mappingPanel->setTuningDisplayed(currentTarget);
    mappingPanel->addTuningWatcher(this);
    mappingPanel->addOptionsWatcher(this);
    addChildComponent(*mappingPanel);

    optionsPanel = std::make_unique<OptionsPanel>(audioProcessor.options());
    optionsPanel->addOptionsWatcher(this);
    addChildComponent(*optionsPanel);
    
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

    logWindow = nullptr;
    optionsPanel = nullptr;
    newTuningPanel = nullptr;
    overviewPanel = nullptr;
    menuPanel = nullptr;

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
    infoBar->setBounds(getLocalBounds().withBottom(menuHeight));
    
    int margin = 5;

    //overviewPanel->setBounds(getLocalBounds().withTop(menuHeight).withTrimmedBottom(menuHeight));
    if (contentComponent != nullptr)
    {
        contentComponent->setBounds(getLocalBounds().withTop(menuHeight).reduced(margin));
    }

}

void MultimapperAudioProcessorEditor::sourceTuningChanged(const std::shared_ptr<MappedTuningTable>& source)
{

}

void MultimapperAudioProcessorEditor::targetTuningChanged(const std::shared_ptr<MappedTuningTable>& target)
{
    infoBar->setDisplayedTuning(target.get());
    intervalListModel->setTuning(target.get());
    overviewPanel->setTuningDisplayed(target.get());
    tuningBackup = std::make_unique<MappedTuningTable>(*audioProcessor.currentTarget());
}

void MultimapperAudioProcessorEditor::targetDefinitionLoaded(TuningChanger* changer, CentsDefinition definition)
{
    commitTuning(definition);   
}

void MultimapperAudioProcessorEditor::targetMappedTuningLoaded(TuningChanger* changer, CentsDefinition tuningDefinition, TuningTableMap::Definition mapDefinition)
{
    
}

void MultimapperAudioProcessorEditor::targetRootFrequencyChanged(TuningChanger* changer, double frequency)
{
    audioProcessor.setTargetTuningRootFrequency(frequency);
}

void MultimapperAudioProcessorEditor::targetMappedTuningRootChanged(TuningChanger* changer, MappedTuningTable::Root root)
{
    audioProcessor.setTargetMappedTuningRoot(root);
}

void MultimapperAudioProcessorEditor::mappingTypeChanged(Everytone::MappingType type)
{
    audioProcessor.setAutoMappingType(type);
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

void MultimapperAudioProcessorEditor::bendModeChanged(Everytone::BendMode newBendMode)
{
    audioProcessor.setBendMode(newBendMode);
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
        Everytone::ShowMenu,
        Everytone::NewTuning,
        Everytone::OpenTuning,
        Everytone::EditReference,
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

    //case Everytone::Save:
    //    result = juce::ApplicationCommandInfo(Everytone::Commands::Save);
    //    result.setInfo("Save", "Save currently edited tuning", "", 0);
    //    result.addDefaultKeypress('s', juce::ModifierKeys::ctrlModifier);
    //    break;

    case Everytone::ShowMenu:
        result = juce::ApplicationCommandInfo(Everytone::Commands::ShowMenu);
        result.setInfo("Show Menu", "Create or edit tunings and other preferences", "Options", 0);
        result.addDefaultKeypress('m', juce::ModifierKeys::ctrlModifier);
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

    case Everytone::EditReference:
        result = juce::ApplicationCommandInfo(Everytone::Commands::EditReference);
        result.setInfo("Edit Reference", "Edit the tuning and mapping reference parameters", "Mapping", 0);
        result.addDefaultKeypress('k', juce::ModifierKeys::ctrlModifier);
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

    //case Everytone::Save:
    //    return performSave(info);

    case Everytone::ShowMenu:
        setContentComponent(menuPanel.get());
        return true;

    case Everytone::NewTuning:
        setContentComponent(newTuningPanel.get());
        return true;
    
    case Everytone::OpenTuning:
        return performOpenTuning(info);

    case Everytone::EditReference:
        setContentComponent(mappingPanel.get());
        return true;

    case Everytone::ShowOptions:
        setContentComponent(optionsPanel.get());
        return true;

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

void MultimapperAudioProcessorEditor::commitTuning(CentsDefinition tuningDefinition)
{
    audioProcessor.loadTuningTarget(tuningDefinition);
    setContentComponent(overviewPanel.get());
}

void MultimapperAudioProcessorEditor::setContentComponent(juce::Component* component)
{
    if (contentComponent != nullptr)
        contentComponent->setVisible(false);

    contentComponent = component;
    contentComponent->setVisible(true);

    infoBar->setButtonBackState(contentComponent != overviewPanel.get());

    resized();
}

void MultimapperAudioProcessorEditor::setupCommands()
{
    registerAllCommandsForTarget(this);
}