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

    auto refChannelLabel = labels.add(new juce::Label("referenceChannelLabel", "MIDI Channel:"));
    addAndMakeVisible(*refChannelLabel);
    refChannelLabel->setJustificationType(juce::Justification::centredRight);
    refChannelLabel->attachToComponent(referenceMidiChannelBox.get(), true);

    referenceMidiNoteBox = std::make_unique<LabelMouseHighlight>("ReferenceMidiNoteBox", juce::String(refNoteBackup));
    addAndMakeVisible(*referenceMidiNoteBox);
    referenceMidiNoteBox->setEditable(false, true);
    referenceMidiNoteBox->setEnabled(!referenceLocked);
    referenceMidiNoteBox->onTextChange = [this]() { tuningReferenceEdited(); };

    auto refNoteLabel = labels.add(new juce::Label("referenceNoteLabel", "MIDI Note:"));
    addAndMakeVisible(*refNoteLabel);
    refNoteLabel->setJustificationType(juce::Justification::centredRight);
    refNoteLabel->attachToComponent(referenceMidiNoteBox.get(), true);

    rootFrequencyBox.reset(new LabelMouseHighlight("rootFrequencyBox", juce::String(mappedRoot.frequency) + " hz"));
    addAndMakeVisible(rootFrequencyBox.get());
    rootFrequencyBox->setEditable(false, true);
    rootFrequencyBox->onTextChange = [this]() { tuningRootFrequencyEdited(); };
    rootFrequencyBackup = mappedRoot.frequency;

    lockReferenceButton = std::make_unique<juce::TextButton>("lockReferenceButton", "Lock Tuning Reference to Mapping Root");
    addAndMakeVisible(*lockReferenceButton);
    lockReferenceButton->setButtonText("Lock");
    lockReferenceButton->setClickingTogglesState(true);
    lockReferenceButton->setToggleState(referenceLocked, juce::NotificationType::dontSendNotification);
    lockReferenceButton->onClick = [this]() { lockReferenceButtonClicked(); };

    rootChannelBackup = mappedRoot.mapping.midiChannel;
    rootMidiChannelBox.reset(new LabelMouseHighlight("rootMidiChannelBox", juce::String(rootChannelBackup)));
    addAndMakeVisible(rootMidiChannelBox.get());
    rootMidiChannelBox->setEditable(false, true);
    rootMidiChannelBox->onTextChange = [this]() { mappingRootEdited(); };

    rootChannelLabel = labels.add(new juce::Label("rootChannelLabel", "MIDI Channel:"));
    rootChannelLabel->setJustificationType(juce::Justification::centredRight);
    rootChannelLabel->attachToComponent(rootMidiChannelBox.get(), true);
    addAndMakeVisible(rootChannelLabel);

    rootNoteBackup = mappedRoot.mapping.midiNote;
    rootMidiNoteBox.reset(new LabelMouseHighlight("rootMidiNoteBox", juce::String(rootNoteBackup)));
    addAndMakeVisible(rootMidiNoteBox.get());
    rootMidiNoteBox->setEditable(false, true);
    rootMidiNoteBox->onTextChange = [this]() { mappingRootEdited(); };

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

    auto mappingLabel = labels.add(new juce::Label("mappingLabel", "Type:"));
    mappingLabel->setJustificationType(juce::Justification::centredRight);
    mappingLabel->attachToComponent(linearMappingButton.get(), true);
    addAndMakeVisible(mappingLabel);

    referenceGroup = std::make_unique<juce::GroupComponent>("ReferenceGroup", "Tuning Reference");
    addAndMakeVisible(*referenceGroup);

    rootGroup = std::make_unique<juce::GroupComponent>("RootGroup", "Mapping Root");
    addAndMakeVisible(*rootGroup);

    mappingRootFrequencyLabel = std::make_unique<juce::Label>("MappingRootFrequencyLabel", "");
    addChildComponent(*mappingRootFrequencyLabel);
    mappingRootFrequencyLabel->setJustificationType(juce::Justification::centredLeft);
    mappingRootFrequencyLabel->setFont(mappingRootFrequencyLabel->getFont().withStyle(juce::Font::FontStyleFlags::italic));

    frequencyGroup = std::make_unique<juce::GroupComponent>("FrequencyGroup", getFrequencyGroupTitle());
    addAndMakeVisible(*frequencyGroup);

    mtsNoteSlider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::IncDecButtons, juce::Slider::TextEntryBoxPosition::TextBoxLeft);
    addAndMakeVisible(*mtsNoteSlider);
    mtsNoteSlider->setName("MtsNoteSlider");
    mtsNoteSlider->setRange(0, 127, 1);
    mtsNoteSlider->setValue(69);
    mtsNoteSlider->onValueChange = [this]() { mtsSnapNoteChanged(); };

    mtsNoteLabel = labels.add(new juce::Label("MtsNoteLabel", "A4"));
    addAndMakeVisible(*mtsNoteLabel);
    mtsNoteLabel->setJustificationType(juce::Justification::centred);
    mtsNoteLabel->attachToComponent(mtsNoteSlider.get(), true);

    mtsSnapButton = std::make_unique<TextButton>("MtsSnapButton", "Snap frequency to selected MTS note");
    addAndMakeVisible(*mtsSnapButton);
    mtsSnapButton->setButtonText("Snap!");
    mtsSnapButton->onClick = [this]() { snapButtonClicked(); };

    mtsSnapGroup = std::make_unique<juce::GroupComponent>("MtsSnapGroup", "MTS Snap");
    addAndMakeVisible(*mtsSnapGroup);
}

MappingPanel::~MappingPanel()
{
    mtsSnapGroup = nullptr;
    mtsSnapButton = nullptr;
    mtsNoteSlider = nullptr;
    mtsNoteLabel = nullptr;
    frequencyGroup = nullptr;
    rootFrequencyBox = nullptr;
    lockReferenceButton = nullptr;
    mappingRootFrequencyLabel = nullptr;
    periodicMappingButton = nullptr;
    linearMappingButton = nullptr;
    rootGroup = nullptr;
    rootMidiNoteBox = nullptr;
    rootMidiChannelBox = nullptr;
    referenceGroup = nullptr;
    referenceMidiNoteBox = nullptr;
    referenceMidiChannelBox = nullptr;
}

Point<float> MappingPanel::getComponentMidPointEdge(juce::Component& component, bool leftEdge)
{
    return component.getBounds()
                    .getCentre()
                    .withX((leftEdge) ? component.getX() : component.getRight())
                    .toFloat();
}

void MappingPanel::paint (juce::Graphics& g)
{
    juce::Path path;
    float lineThickness = 1.0f;

    auto referenceMid = getComponentMidPointEdge(*referenceGroup, false);
    auto frequencyMid = getComponentMidPointEdge(*frequencyGroup, true);

    if (lockReferenceButton->getToggleState())
    {
        auto rootMid = getComponentMidPointEdge(*rootGroup, false);
        auto lockLeftMid = getComponentMidPointEdge(*lockReferenceButton, true);
        auto lockRightMid = getComponentMidPointEdge(*lockReferenceButton, false);

        auto leftLockMarginMidX = (referenceGroup->getRight() + lockReferenceButton->getX()) * 0.5f;

        auto refLeftLockMarginMid = referenceMid.withX(leftLockMarginMidX);
        auto rootLeftLockMarginMid = rootMid.withX(leftLockMarginMidX);
        auto controlMidLeftLockMid = lockLeftMid.withX(leftLockMarginMidX);

        path.addLineSegment(juce::Line<float>(referenceMid, refLeftLockMarginMid), lineThickness);
        path.addLineSegment(juce::Line<float>(refLeftLockMarginMid, controlMidLeftLockMid), lineThickness);

        path.addLineSegment(juce::Line<float>(rootMid, rootLeftLockMarginMid), lineThickness);
        path.addLineSegment(juce::Line<float>(rootLeftLockMarginMid, controlMidLeftLockMid), lineThickness);

        path.addLineSegment(juce::Line<float>(controlMidLeftLockMid, lockLeftMid), lineThickness);


        auto rightLockMarginMidX = (lockReferenceButton->getRight() + frequencyGroup->getX()) * 0.5f;
        auto rightLockMarginMid = lockRightMid.withX(rightLockMarginMidX);
        path.addLineSegment(juce::Line<float>(lockRightMid, rightLockMarginMid), lineThickness);

        auto rightLockMarginFreq = rightLockMarginMid.withY(frequencyMid.getY());
        path.addLineSegment(juce::Line<float>(rightLockMarginMid, rightLockMarginFreq), lineThickness);
        path.addLineSegment(juce::Line<float>(rightLockMarginFreq, frequencyMid), lineThickness);
    }
    else
    {
        path.addLineSegment(juce::Line<float>(referenceMid, frequencyMid), lineThickness);
    }

    g.setColour(juce::Colours::lightgrey);
    g.strokePath(path, juce::PathStrokeType(lineThickness));
}

void MappingPanel::resized()
{
    auto w = getWidth();
    auto h = getHeight();

    auto margin = w * 0.01f;

    auto controlFont = referenceMidiChannelBox->getFont();
    int controlWidth = controlFont.getStringWidth("9999");
    auto controlHeight = controlFont.getHeight() + margin * 2;

    int buttonWidth = juce::roundToInt(w * 0.1f);

    int referenceLabelWidth = controlFont.getStringWidth(rootChannelLabel->getText());

    int boundMargin = margin * 2;
    int controlYMargin = margin * 3;
    int controlXMargin = margin * 4;

    auto controlBoxWidth = (w - buttonWidth - controlXMargin * 2) * 0.4f;

    auto refBoundsHeight = controlHeight * 2 + margin * 4;
    auto refBounds = juce::Rectangle<int>(boundMargin, margin, controlBoxWidth, refBoundsHeight);

    auto referenceControlsX = refBounds.getX() + referenceLabelWidth + controlXMargin;
    referenceMidiChannelBox->setBounds(referenceControlsX, refBounds.getY() + controlYMargin, controlWidth, controlHeight);
    referenceMidiNoteBox->setBounds(referenceControlsX, referenceMidiChannelBox->getBottom(), controlWidth, controlHeight);
    referenceGroup->setBounds(refBounds);

    auto rootBoundsHeight = controlHeight * 3 + margin * 4;
    auto rootBounds = juce::Rectangle<int>(boundMargin, refBounds.getBottom() + boundMargin, controlBoxWidth, rootBoundsHeight);
    rootMidiChannelBox->setBounds(referenceControlsX, rootBounds.getY() + controlYMargin, controlWidth, controlHeight);
    rootMidiNoteBox->setBounds(referenceControlsX, rootMidiChannelBox->getBottom(), controlWidth, controlHeight);

    auto buttonHeight = controlHeight * 0.8;
    auto mapButtonMargin = controlFont.getStringWidth("Type: ") + margin;
    linearMappingButton->setBounds(boundMargin + mapButtonMargin + margin, rootMidiNoteBox->getBottom(), buttonWidth, buttonHeight);
    periodicMappingButton->setBounds(linearMappingButton->getRight(), linearMappingButton->getY(), buttonWidth, buttonHeight);
    rootGroup->setBounds(rootBounds);

    lockReferenceButton->setSize(controlWidth + boundMargin * 2, controlHeight);
    lockReferenceButton->setCentrePosition(w * 0.5, h * 0.5);

    auto frequencyBounds = refBounds.withPosition(w - boundMargin - controlBoxWidth, refBounds.getY());

    auto freqBoxWidth = rootFrequencyBox->getFont().getStringWidth("9999.999 Hz");
    rootFrequencyBox->setSize(freqBoxWidth, controlHeight);
    rootFrequencyBox->setCentrePosition(frequencyBounds.getCentre());

    frequencyGroup->setBounds(frequencyBounds);


    auto mtsSnapBounds = rootBounds.withPosition(frequencyBounds.getX(), rootBounds.getY());

    auto mtsNoteLabelWidth = mtsNoteLabel->getFont().getStringWidth("999");
    juce::FlexItem::Margin mtsNoteSliderMargin(margin, 0, 0, mtsNoteLabelWidth);

    auto sliderValueWidth = w * 0.11f;// mtsNoteLabelFont.getStringWidth("9999");
    auto sliderButtonsWidth = w * 0.09f;// sliderValueWidth * 0.3f;
    auto sliderWidth = sliderValueWidth + sliderButtonsWidth;

    auto mtsSnapMargin = (mtsSnapBounds.getHeight() - controlHeight * 2) * 0.5;

    mtsNoteSlider->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxLeft, false, sliderValueWidth, controlHeight);
    mtsNoteSlider->setBounds(mtsSnapBounds.getX() + mtsNoteLabelWidth + controlXMargin, mtsSnapBounds.getY() + mtsSnapMargin, sliderWidth, controlHeight);

    mtsSnapButton->setBounds(mtsNoteSlider->getBounds().translated(0, controlHeight));

    mtsSnapGroup->setBounds(mtsSnapBounds);
}

void MappingPanel::lockReferenceButtonClicked()
{
    bool locked = lockReferenceButton->getToggleState();
    refChannelBackup = rootChannelBackup;
    refNoteBackup = rootNoteBackup;
    setLockState(locked, true);
}

void MappingPanel::setLockState(bool isLocked, bool sendChangeMessage)
{
    referenceMidiChannelBox->setText(juce::String(refChannelBackup), juce::NotificationType::dontSendNotification);
    referenceMidiChannelBox->setEnabled(!isLocked);

    referenceMidiNoteBox->setText(juce::String(refNoteBackup), juce::NotificationType::dontSendNotification);
    referenceMidiNoteBox->setEnabled(!isLocked);

    frequencyGroup->setText(getFrequencyGroupTitle());
    frequencyGroup->repaint();

    mappingRootFrequencyLabel->setVisible(!isLocked);
    if (!isLocked && tuning != nullptr)
    {
        mappingRootFrequencyLabel->setText(juce::String(tuning->frequencyFromRoot(0)), juce::NotificationType::dontSendNotification);
    }

    repaint();

    if (sendChangeMessage)
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
    tuning = mappedTuning;
    auto mappingRoot = tuning->getRoot();

    rootFrequencyBackup = mappingRoot.frequency;
    rootFrequencyBox->setText(juce::String(mappingRoot.frequency) + " Hz", juce::NotificationType::dontSendNotification);

    rootChannelBackup = mappingRoot.mapping.midiChannel;
    rootMidiChannelBox->setText(juce::String(rootChannelBackup), juce::NotificationType::dontSendNotification);

    rootNoteBackup = mappingRoot.mapping.midiNote;
    rootMidiNoteBox->setText(juce::String(rootNoteBackup), juce::NotificationType::dontSendNotification);

    bool referenceLocked = mappingRoot.tuningReference.isInvalid();
    lockReferenceButton->setToggleState(referenceLocked, juce::NotificationType::dontSendNotification);

    refChannelBackup = (referenceLocked) ? rootChannelBackup : mappingRoot.tuningReference.midiChannel;
    refNoteBackup = (referenceLocked) ? rootNoteBackup : mappingRoot.tuningReference.midiNote;
    
    setLockState(referenceLocked, false);
}

void MappingPanel::tuningReferenceEdited()
{
    bool referenceLocked = lockReferenceButton->getToggleState();

    if (referenceLocked)
    {
        refChannelBackup = rootChannelBackup;
        refNoteBackup = rootNoteBackup;
        
        referenceMidiChannelBox->setText(juce::String(refChannelBackup), juce::NotificationType::dontSendNotification);
        referenceMidiNoteBox->setText(juce::String(refNoteBackup), juce::NotificationType::dontSendNotification);

        tuningWatchers.call(&TuningWatcher::targetTuningReferenceChanged, this, MappedTuningTable::FrequencyReference());
        return;
    }

    auto channelInputTokens = juce::StringArray::fromTokens(referenceMidiChannelBox->getText().trim(), " ", "");
    auto channelInput = channelInputTokens[0].getDoubleValue();

    refChannelBackup = ChannelInMidiRange(channelInput) ? channelInput
                                                        : refChannelBackup;

    auto noteInputTokens = juce::StringArray::fromTokens(referenceMidiNoteBox->getText().trim(), " ", "");
    auto noteInput = noteInputTokens[0].getIntValue();

    refNoteBackup = NoteInMidiRange(noteInput) ? noteInput
                                               : refNoteBackup;
    
    referenceMidiChannelBox->setText(juce::String(refChannelBackup), juce::NotificationType::dontSendNotification);
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

    rootChannelBackup = ChannelInMidiRange(channelInput) ? channelInput
                                                         : rootChannelBackup;
    rootMidiChannelBox->setText(juce::String(rootChannelBackup), juce::NotificationType::dontSendNotification);


    auto noteInputTokens = juce::StringArray::fromTokens(rootMidiNoteBox->getText().trim(), " ", "");
    auto noteInput = noteInputTokens[0].getIntValue();

    rootNoteBackup = NoteInMidiRange(noteInput) ? noteInput
                                                : rootNoteBackup;
    rootMidiNoteBox->setText(juce::String(rootNoteBackup), juce::NotificationType::dontSendNotification);

    TuningTableMap::Root newRoot = { rootChannelBackup, rootNoteBackup };

    if (lockReferenceButton->getToggleState()) // If Locked
    {
        MappedTuningTable::Root mappedRoot = { MappedTuningTable::FrequencyReference(), rootFrequencyBackup, newRoot };
        tuningWatchers.call(&TuningWatcher::targetMappedTuningRootChanged, this, mappedRoot);
        return;
    }

    tuningWatchers.call(&TuningWatcher::targetMappingRootChanged, this, newRoot);
}

void MappingPanel::mtsSnapNoteChanged()
{
    auto mtsNote = (int)mtsNoteSlider->getValue();
    auto label = juce::MidiMessage::getMidiNoteName(mtsNote, true, true, 4);
    mtsNoteLabel->setText(label, juce::NotificationType::dontSendNotification);
}

void MappingPanel::snapButtonClicked()
{
    auto mtsNote = (int)mtsNoteSlider->getValue();
    auto frequency = mtsToFrequency(mtsNote);
    rootFrequencyBox->setText(juce::String(frequency) + " Hz", juce::NotificationType(sendNotification));
}

juce::String MappingPanel::getFrequencyGroupTitle() const
{
    return (lockReferenceButton->getToggleState()) ? juce::String("Root Frequency") 
                                                   : juce::String("Reference Frequency");
}
