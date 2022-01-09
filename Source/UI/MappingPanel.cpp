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

    refChannelLabel = labels.add(new juce::Label("referenceChannelLabel", "MIDI Channel:"));
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

    //auto rootFrequencyLabel = labels.add(new juce::Label("rootFrequencyLabel", "Frequency:"));
    //rootFrequencyLabel->setJustificationType(juce::Justification::centredLeft);
    //rootFrequencyLabel->attachToComponent(rootFrequencyBox.get(), false);
    //addAndMakeVisible(rootFrequencyLabel);

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

void MappingPanel::paint (juce::Graphics& g)
{
}

void MappingPanel::resized()
{
    auto w = getWidth();
    auto h = getHeight();

    int halfHeight = h * 0.5f;

    auto margin = 0.01f;
    margin *= (w > h) ? w : h;

    auto controlFont = referenceMidiChannelBox->getFont();
    int controlWidth = controlFont.getStringWidth("9999");
    auto controlHeight = controlFont.getHeight() + margin * 2;

    int buttonWidth = juce::roundToInt(w * 0.1f);

    int referenceLabelWidth = controlFont.getStringWidth(rootChannelLabel->getText());
    //juce::FlexItem::Margin referenceMargin(0, 0, 0, referenceLabelWidth);

    int controlXMargin = margin * 4;
    int controlYMargin = margin * 3;
    int interControlYMargin = margin * 0.5;

    auto controlBoxWidth = (w - buttonWidth - controlXMargin * 2) * 0.4f;


    auto boundMargin = margin * 2;
    auto refBoundsWidth = referenceLabelWidth + controlWidth + margin * 2;
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
    //juce::Rectangle<int>(w - boundMargin - controlBoxWidth, boundMargin, controlBoxWidth, controlHeight + boundMargin * 2);

    auto freqBoxWidth = rootFrequencyBox->getFont().getStringWidth("9999.999 Hz");
    rootFrequencyBox->setSize(freqBoxWidth, controlHeight);
    rootFrequencyBox->setCentrePosition(frequencyBounds.getCentre());

    frequencyGroup->setBounds(frequencyBounds);


    auto mtsSnapBounds = rootBounds.withPosition(frequencyBounds.getX(), rootBounds.getY());
    //juce::Rectangle<int>(, h - mtsBoundsHeight - margin, controlBoxWidth, mtsBoundsHeight);

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

    //juce::FlexBox main;
    //main.flexDirection = juce::FlexBox::Direction::row;
    ////main.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;

    //// Mapping Root & Tuning Reference column

    //juce::FlexBox mapRefColumnFlex;
    //mapRefColumnFlex.flexDirection = juce::FlexBox::Direction::column;
    ////mapRefColumnFlex.justifyContent = juce::FlexBox::JustifyContent::center;

    //int mapRefSectionRows = 5;
    //int mapRefControlHeight = h / (mapRefSectionRows * 2);

    //juce::FlexBox referenceBox;
    //referenceBox.flexDirection = juce::FlexBox::Direction::column;
    //referenceBox.justifyContent = juce::FlexBox::JustifyContent::center;
    //auto rootBox = juce::FlexBox(referenceBox);

    //referenceBox.items.add(juce::FlexItem(controlWidth, controlHeight, *referenceMidiChannelBox).withMargin(referenceMargin));
    //referenceBox.items.add(juce::FlexItem(controlWidth, controlHeight, *referenceMidiNoteBox).withMargin(referenceMargin));

    //rootBox.items.add(juce::FlexItem(controlWidth, controlHeight, *rootMidiChannelBox).withMargin(referenceMargin));
    //rootBox.items.add(juce::FlexItem(controlWidth, controlHeight, *rootMidiNoteBox).withMargin(referenceMargin));

    //int buttonHeight = juce::roundToInt(controlHeight * 0.8f);

    //juce::FlexBox mappingRow;
    //juce::FlexItem::Margin mappingRowMargin(0, 0, 0, w * 0.08f);
    //mappingRow.justifyContent = juce::FlexBox::JustifyContent::flexStart;
    //mappingRow.items.add(juce::FlexItem(buttonWidth, buttonHeight, *linearMappingButton));
    //mappingRow.items.add(juce::FlexItem(buttonWidth, buttonHeight, *periodicMappingButton));
    //rootBox.items.add(juce::FlexItem(buttonWidth * 2, buttonHeight, mappingRow).withMargin(mappingRowMargin));

    //mapRefColumnFlex.items.add(juce::FlexItem(referenceBox).withMinWidth(controlWidth).withMinHeight(halfHeight));
    //mapRefColumnFlex.items.add(juce::FlexItem(rootBox).withMinWidth(controlWidth).withMinHeight(halfHeight));
    //main.items.add(juce::FlexItem(controlBoxWidth, h, mapRefColumnFlex));

    //juce::FlexBox lockBox;
    //auto lockBoxMarginHeight = (h - controlHeight) * 0.5;
    //juce::FlexItem::Margin lockBoxMargin = (lockBoxMarginHeight, margin, lockBoxMarginHeight, margin);
    //lockBox.items.add(juce::FlexItem(buttonWidth, controlHeight, *lockReferenceButton).withMaxHeight(controlHeight).withMargin(lockBoxMargin));
    //main.items.add(juce::FlexItem(buttonWidth + margin * 2, h, lockBox));

    //juce::FlexBox freqColumnBox;
    //freqColumnBox.flexDirection = juce::FlexBox::Direction::column;
    //freqColumnBox.justifyContent = juce::FlexBox::JustifyContent::center;
    ////freqColumnBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
    ////freqColumnBox.alignContent = juce::FlexBox::AlignContent::flexEnd;

    //auto freqControlWidth = rootFrequencyBox->getFont().getStringWidth("99999 Hz");
    //auto freqMarginX = (controlBoxWidth - freqControlWidth) * 0.5f;
    //auto freqMarginY = (halfHeight - controlHeight) * 0.5;
    //juce::FlexItem::Margin freqMargin(freqMarginY, freqMarginX, freqMarginY, freqMarginY);
    //freqColumnBox.items.add(juce::FlexItem(freqControlWidth, controlHeight, *rootFrequencyBox));// .withMargin(freqMargin));

    //juce::FlexBox mtsSnapBox;
    //mtsSnapBox.flexDirection = juce::FlexBox::Direction::column;

    ////auto mtsLabelFont = mtsSnapLabel->getFont();
    //auto mtsNoteLabelFont = mtsNoteLabel->getFont();
    //auto mtsNoteLabelWidth = mtsNoteLabelFont.getStringWidth("999");
    //juce::FlexItem::Margin mtsNoteSliderMargin(margin, 0, 0, mtsNoteLabelWidth);

    //auto sliderValueWidth = w * 0.11f;// mtsNoteLabelFont.getStringWidth("9999");
    //auto sliderButtonsWidth = w * 0.09f;// sliderValueWidth * 0.3f;
    //auto sliderWidth = sliderValueWidth + sliderButtonsWidth;

    //mtsNoteSlider->setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxLeft, false, sliderValueWidth, controlHeight);

    //mtsSnapBox.items.add(juce::FlexItem(sliderWidth, controlHeight, *mtsNoteSlider).withMaxWidth(sliderWidth));// .withMargin(mtsNoteSliderMargin));
    //mtsSnapBox.items.add(juce::FlexItem(sliderValueWidth, controlHeight, *mtsSnapButton).withMaxWidth(sliderValueWidth));// .withMargin(mtsNoteSliderMargin));

    //freqColumnBox.items.add(juce::FlexItem(sliderWidth, halfHeight, mtsSnapBox));

    //main.items.add(juce::FlexItem(controlBoxWidth, h, freqColumnBox));
    //
    //main.performLayout(getLocalBounds());

    //mtsNoteSlider->setSize(sliderWidth, controlHeight);



    //auto groupXMargin = w * 0.02;
    //auto groupYMargin = h * 0.07f;

    //auto rootBounds = juce::Rectangle<int>(rootChannelLabel->getPosition(), periodicMappingButton->getBounds().getBottomRight());
    //rootBounds = rootBounds.withRight(rootBounds.getRight() + groupXMargin);
    //rootGroup->setBounds(rootBounds.expanded(0, groupYMargin));
    //
    //auto refBounds = rootBounds.withPosition(refChannelLabel->getPosition()).withHeight(controlHeight * 2);
    //referenceGroup->setBounds(refBounds.expanded(0, groupYMargin));

    //auto mapFreqFont = mappingRootFrequencyLabel->getFont();
    //auto mapFreqCenterX = (mapFreqFont.getStringWidth(mappingRootFrequencyLabel->getText())) * 0.5 + rootBounds.getRight();
    //auto mapFreqCenterY = (rootMidiChannelBox->getY() + rootMidiNoteBox->getBottom()) * 0.5;
    //mappingRootFrequencyLabel->setCentrePosition(juce::Point<int>(mapFreqCenterX, mapFreqCenterY));

    //auto freqBounds = rootFrequencyBox->getBounds().expanded(groupXMargin, groupYMargin);
    //frequencyGroup->setBounds(freqBounds);

    //auto mtsSnapBounds = juce::Rectangle<int>(mtsNoteLabel->getPosition(), mtsNoteSlider->getBounds().getBottomRight())
    //                                         .withBottom(mtsSnapButton->getBottom())
    //                                         .expanded(groupXMargin * 2, groupYMargin);
    //mtsSnapGroup->setBounds(mtsSnapBounds);
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
    
    // TODO note number to note label
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
