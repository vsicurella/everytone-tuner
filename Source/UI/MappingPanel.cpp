/*
  ==============================================================================

    MappingPanel.cpp
    Created: 28 Dec 2021 10:51:58pm
    Author:  Vincenzo

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MappingPanel.h"

//==============================================================================
MappingPanel::MappingPanel(Everytone::Options options, MappedTuningTable* tuningIn)
{
    MappedTuningTable::Root mappedRoot;
    if (tuningIn != nullptr)
        mappedRoot = tuningIn->getRoot();

    bool referenceLocked = mappedRoot.tuningReference.isInvalid();

    refChannelBackup = referenceLocked ? mappedRoot.mapping.midiChannel
                                       : mappedRoot.tuningReference.midiChannel;

    refNoteBackup = referenceLocked ? mappedRoot.mapping.midiNote
                                    : mappedRoot.tuningReference.midiNote;

    referenceMidiChannelBox = std::make_unique<LabelMouseHighlight>("ReferenceMidiChannelBox", juce::String(refChannelBackup));
    addAndMakeVisible(*referenceMidiChannelBox);
    referenceMidiChannelBox->setEditable(false, true);
    referenceMidiChannelBox->setEnabled(!referenceLocked);
    referenceMidiChannelBox->onTextChange = [this]() { tuningReferenceEdited(); };

    auto refChannelLabel = labels.add(new juce::Label("referenceChannelLabel", "Midi Channel:"));
    addAndMakeVisible(*refChannelLabel);
    refChannelLabel->setJustificationType(juce::Justification::centredRight);
    refChannelLabel->attachToComponent(referenceMidiChannelBox.get(), true);

    referenceMidiNoteBox = std::make_unique<LabelMouseHighlight>("ReferenceMidiNoteBox", juce::String(refNoteBackup));
    addAndMakeVisible(*referenceMidiNoteBox);
    referenceMidiNoteBox->setEditable(false, true);
    referenceMidiNoteBox->setEnabled(!referenceLocked);
    referenceMidiNoteBox->onTextChange = [this]() { tuningReferenceEdited(); };

    auto refNoteLabel = labels.add(new juce::Label("referenceNoteLabel", "Midi Note:"));
    addAndMakeVisible(*refNoteLabel);
    refNoteLabel->setJustificationType(juce::Justification::centredRight);
    refNoteLabel->attachToComponent(referenceMidiNoteBox.get(), true);

    rootFrequencyBox.reset(new LabelMouseHighlight("rootFrequencyBox", juce::String(mappedRoot.frequency) + " hz"));
    addAndMakeVisible(rootFrequencyBox.get());
    rootFrequencyBox->setEditable(false, true);
    rootFrequencyBox->onTextChange = [this]() { tuningRootFrequencyEdited(); };
    rootFrequencyBackup = mappedRoot.frequency;

    auto rootFrequencyLabel = labels.add(new juce::Label("rootFrequencyLabel", "Frequency:"));
    rootFrequencyLabel->setJustificationType(juce::Justification::centredRight);
    rootFrequencyLabel->attachToComponent(rootFrequencyBox.get(), true);
    addAndMakeVisible(rootFrequencyLabel);

    lockReferenceButton = std::make_unique<juce::TextButton>("lockReferenceButton", "Lock Tuning Reference to Mapping Root");
    addAndMakeVisible(*lockReferenceButton);
    lockReferenceButton->setButtonText("Lock Reference");
    lockReferenceButton->setClickingTogglesState(true);
    lockReferenceButton->setToggleState(referenceLocked, juce::NotificationType::dontSendNotification);
    lockReferenceButton->onClick = [this]() { lockReferenceButtonClicked(); };

    rootChannelBackup = mappedRoot.mapping.midiChannel;
    rootMidiChannelBox.reset(new LabelMouseHighlight("rootMidiChannelBox", juce::String(rootChannelBackup)));
    addAndMakeVisible(rootMidiChannelBox.get());
    rootMidiChannelBox->setEditable(false, true);
    rootMidiChannelBox->onTextChange = [this]() { tuningReferenceEdited(); };

    auto rootChannelLabel = labels.add(new juce::Label("rootChannelLabel", "MIDI Channel:"));
    rootChannelLabel->setJustificationType(juce::Justification::centredRight);
    rootChannelLabel->attachToComponent(rootMidiChannelBox.get(), true);
    addAndMakeVisible(rootChannelLabel);

    rootNoteBackup = mappedRoot.mapping.midiNote;
    rootMidiNoteBox.reset(new LabelMouseHighlight("rootMidiNoteBox", juce::String(rootNoteBackup)));
    addAndMakeVisible(rootMidiNoteBox.get());
    rootMidiNoteBox->setEditable(false, true);
    rootMidiNoteBox->onTextChange = [this]() { tuningReferenceEdited(); };

    auto rootNoteLabel = labels.add(new juce::Label("rootNoteLabel", "MIDI Note:"));
    rootNoteLabel->setJustificationType(juce::Justification::centredRight);
    rootNoteLabel->attachToComponent(rootMidiNoteBox.get(), true);
    addAndMakeVisible(rootNoteLabel);

    auto mappingButtonCallback = [this]() { mappingTypeButtonClicked(); };

    linearMappingButton.reset(new juce::TextButton("linearMappingButton"));
    addAndMakeVisible(linearMappingButton.get());
    linearMappingButton->setButtonText("Linear");
    linearMappingButton->setConnectedEdges(juce::Button::ConnectedEdgeFlags::ConnectedOnRight);
    linearMappingButton->setClickingTogglesState(true);
    linearMappingButton->setToggleState(options.mappingType == Everytone::MappingType::Linear, juce::NotificationType::dontSendNotification);
    linearMappingButton->setRadioGroupId(10);
    linearMappingButton->onClick = mappingButtonCallback;

    periodicMappingButton.reset(new juce::TextButton("periodicMappingButton"));
    addAndMakeVisible(periodicMappingButton.get());
    periodicMappingButton->setButtonText("Periodic");
    periodicMappingButton->setConnectedEdges(juce::Button::ConnectedEdgeFlags::ConnectedOnLeft);
    periodicMappingButton->setClickingTogglesState(true);
    periodicMappingButton->setToggleState(options.mappingType == Everytone::MappingType::Periodic, juce::NotificationType::dontSendNotification);
    periodicMappingButton->setRadioGroupId(10);

    auto mappingLabel = labels.add(new juce::Label("mappingLabel", "Mapping:"));
    mappingLabel->setJustificationType(juce::Justification::centredRight);
    mappingLabel->attachToComponent(linearMappingButton.get(), true);
    addAndMakeVisible(mappingLabel);

    referenceGroup = std::make_unique<juce::GroupComponent>("ReferenceGroup", "Tuning Reference");
    addAndMakeVisible(*referenceGroup);

    rootGroup = std::make_unique<juce::GroupComponent>("RootGroup", "Mapping Root");
    addAndMakeVisible(*rootGroup);
}

MappingPanel::~MappingPanel()
{
    periodicMappingButton = nullptr;
    linearMappingButton = nullptr;
    rootGroup = nullptr;
    rootMidiNoteBox = nullptr;
    rootMidiChannelBox = nullptr;
    lockReferenceButton = nullptr;
    referenceGroup = nullptr;
    rootFrequencyBox = nullptr;
    referenceMidiNoteBox = nullptr;
    referenceMidiChannelBox = nullptr;
}

void MappingPanel::paint (juce::Graphics& g)
{
}

void MappingPanel::resized()
{
    auto w = getWidth();
    auto h = getHeight();

    auto margin = 0.01;
    margin *= (w > h) ? w : h;

    auto controlHeight = (h - margin * 5) * 0.2f;

    int referenceControlWidth = w * 0.18;
    int referenceWidth = w * 0.38;
    int referenceLabelWidth = w * 0.15;
    juce::FlexItem::Margin referenceMargin(0, 0, 0, referenceLabelWidth);


    juce::FlexBox referenceBox;
    referenceBox.flexDirection = juce::FlexBox::Direction::column;

    referenceBox.items.add(juce::FlexItem(referenceControlWidth, controlHeight, *referenceMidiChannelBox).withMargin(referenceMargin));
    referenceBox.items.add(juce::FlexItem(referenceControlWidth, controlHeight, *referenceMidiNoteBox).withMargin(referenceMargin));
    referenceBox.items.add(juce::FlexItem(referenceControlWidth, controlHeight, *rootFrequencyBox).withMargin(referenceMargin));

    juce::FlexBox rootBox;
    rootBox.flexDirection = juce::FlexBox::Direction::column;

    rootBox.items.add(juce::FlexItem(referenceControlWidth, controlHeight, *rootMidiChannelBox).withMargin(referenceMargin));
    rootBox.items.add(juce::FlexItem(referenceControlWidth, controlHeight, *rootMidiNoteBox).withMargin(referenceMargin));

    int buttonWidth = juce::roundToInt(w * 0.15);
    int buttonHeight = juce::roundToInt(controlHeight * 0.6);

    juce::FlexBox mappingRow;
    mappingRow.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    mappingRow.items.add(juce::FlexItem(buttonWidth, buttonHeight, *linearMappingButton));
    mappingRow.items.add(juce::FlexItem(buttonWidth, buttonHeight, *periodicMappingButton));
    rootBox.items.add(juce::FlexItem(buttonWidth * 2, buttonHeight, mappingRow).withMargin(referenceMargin));

    juce::FlexBox main;
    main.flexDirection = juce::FlexBox::Direction::row;

    juce::FlexItem::Margin mainMargin(getHeight() * 0.2f, 0, 0, 0);

    main.items.add(juce::FlexItem(getWidth() * 0.4f, getHeight(), referenceBox).withMargin(mainMargin));
    main.items.add(juce::FlexItem(getWidth() * 0.2f, controlHeight, *lockReferenceButton).withAlignSelf(juce::FlexItem::AlignSelf::center));
    main.items.add(juce::FlexItem(getWidth() * 0.4f, getHeight(), rootBox).withMargin(mainMargin));
    
    main.performLayout(getLocalBounds());

    referenceGroup->setBounds(getLocalBounds().withRight(getWidth() * 0.4f));
    rootGroup->setBounds(getLocalBounds().withTrimmedLeft(getWidth() * 0.6f));
}

void MappingPanel::lockReferenceButtonClicked()
{
    bool locked = lockReferenceButton->getToggleState();
    setReferenceLockState(locked, true);
}

void MappingPanel::setReferenceLockState(bool isLocked, bool sendChangeMessage)
{
    auto refChannel = (isLocked) ? rootChannelBackup : refChannelBackup;
    referenceMidiChannelBox->setText(juce::String(refChannel), juce::NotificationType::dontSendNotification);
    referenceMidiChannelBox->setEnabled(!isLocked);

    auto refNote = (isLocked) ? rootNoteBackup : refNoteBackup;
    referenceMidiNoteBox->setText(juce::String(refNote), juce::NotificationType::dontSendNotification);
    referenceMidiNoteBox->setEnabled(!isLocked);

    if (isLocked && sendChangeMessage)
        tuningReferenceEdited();
}

void MappingPanel::mappingTypeButtonClicked()
{
    auto type = (linearMappingButton->getToggleState())
        ? Everytone::MappingType::Linear
        : Everytone::MappingType::Periodic;

    optionsWatchers.call(&OptionsWatcher::mappingTypeChanged, type);
}

void MappingPanel::setTuningDisplayed(const MappedTuningTable* mappedTuning)
{
    auto mappingRoot = mappedTuning->getRoot();

    rootFrequencyBackup = mappingRoot.frequency;
    rootFrequencyBox->setText(juce::String(mappingRoot.frequency), juce::NotificationType::dontSendNotification);

    rootChannelBackup = mappingRoot.mapping.midiChannel;
    rootMidiChannelBox->setText(juce::String(rootChannelBackup), juce::NotificationType::dontSendNotification);

    rootNoteBackup = mappingRoot.mapping.midiNote;
    rootMidiNoteBox->setText(juce::String(rootNoteBackup), juce::NotificationType::dontSendNotification);

    bool referenceLocked = mappingRoot.tuningReference.isInvalid();
    lockReferenceButton->setToggleState(referenceLocked, juce::NotificationType::dontSendNotification);

    refChannelBackup = (referenceLocked) ? rootChannelBackup : mappingRoot.tuningReference.midiChannel;
    refNoteBackup = (referenceLocked) ? rootNoteBackup : mappingRoot.tuningReference.midiNote;
    
    setReferenceLockState(referenceLocked, false);
}

void MappingPanel::tuningReferenceEdited()
{
    auto channelInputTokens = juce::StringArray::fromTokens(referenceMidiChannelBox->getText().trim(), " ", "");
    auto channelInput = channelInputTokens[0].getDoubleValue();

    refChannelBackup = (channelInput >= 1 && channelInput <= 16) ? channelInput
                                                                 : refChannelBackup;
    referenceMidiChannelBox->setText(juce::String(refChannelBackup), juce::NotificationType::dontSendNotification);


    auto noteInputTokens = juce::StringArray::fromTokens(referenceMidiNoteBox->getText().trim(), " ", "");
    auto noteInput = noteInputTokens[0].getIntValue();

    refNoteBackup = (noteInput >= 0 && noteInput < 128) ? noteInput
                                                        : refNoteBackup;
    referenceMidiNoteBox->setText(juce::String(refNoteBackup), juce::NotificationType::dontSendNotification);

    MappedTuningTable::FrequencyReference newReference = { refChannelBackup, refNoteBackup };
    tuningWatchers.call(&TuningWatcher::targetTuningReferenceChanged, this, newReference);
}

void MappingPanel::tuningRootFrequencyEdited()
{
    auto inputTokens = juce::StringArray::fromTokens(rootFrequencyBox->getText().trim(), " ", "");
    auto freqInput = inputTokens[0].getDoubleValue();

    rootFrequencyBackup = (freqInput >= 8.0 && freqInput < 14000.0) ? freqInput
                                                                    : rootFrequencyBackup;

    rootFrequencyBox->setText(juce::String(rootFrequencyBackup) + " hz", juce::NotificationType::dontSendNotification);
    tuningWatchers.call(&TuningWatcher::targetRootFrequencyChanged, this, rootFrequencyBackup);
}

void MappingPanel::mappingRootEdited()
{
    auto channelInputTokens = juce::StringArray::fromTokens(rootMidiChannelBox->getText().trim(), " ", "");
    auto channelInput = channelInputTokens[0].getDoubleValue();

    rootChannelBackup = (channelInput >= 1 && channelInput <= 16) ? channelInput
                                                                  : rootChannelBackup;
    referenceMidiChannelBox->setText(juce::String(rootChannelBackup), juce::NotificationType::dontSendNotification);


    auto noteInputTokens = juce::StringArray::fromTokens(rootMidiNoteBox->getText().trim(), " ", "");
    auto noteInput = noteInputTokens[0].getIntValue();

    rootNoteBackup = (noteInput >= 0 && noteInput < 128) ? noteInput
                                                         : rootNoteBackup;
    rootMidiNoteBox->setText(juce::String(rootNoteBackup), juce::NotificationType::dontSendNotification);

    TuningTableMap::Root newRoot = { rootChannelBackup, rootNoteBackup };
    tuningWatchers.call(&TuningWatcher::targetMappingRootChanged, this, newRoot);
}

