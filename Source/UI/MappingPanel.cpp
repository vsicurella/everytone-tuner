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
	rootMidiChannelBox.reset(new juce::Label("rootMidiChannelBox", "1"));
	addAndMakeVisible(rootMidiChannelBox.get());
	rootMidiChannelBox->setEditable(false, true);
	rootMidiChannelBox->onEditorShow = [&]() { rootChannelBackup = rootMidiChannelBox->getText(); };
	rootMidiChannelBox->onTextChange = [&]() { tuningReferenceEdited(); };

	auto rootChannelLabel = labels.add(new juce::Label("rootChannelLabel", "Root MIDI Channel:"));
	rootChannelLabel->setJustificationType(juce::Justification::centredRight);
	rootChannelLabel->attachToComponent(rootMidiChannelBox.get(), true);
	addAndMakeVisible(rootChannelLabel);

	rootMidiNoteBox.reset(new juce::Label("rootMidiNoteBox", "60"));
	addAndMakeVisible(rootMidiNoteBox.get());
	rootMidiNoteBox->setEditable(false, true);
	rootMidiNoteBox->onEditorShow = [&]() { rootNoteBackup = rootMidiNoteBox->getText(); };
	rootMidiNoteBox->onTextChange = [&]() { tuningReferenceEdited(); };

	auto rootNoteLabel = labels.add(new juce::Label("rootNoteLabel", "Root MIDI Note:"));
	rootNoteLabel->setJustificationType(juce::Justification::centredRight);
	rootNoteLabel->attachToComponent(rootMidiNoteBox.get(), true);
	addAndMakeVisible(rootNoteLabel);

	rootFrequencyBox.reset(new juce::Label("rootFrequencyBox", "263"));
	addAndMakeVisible(rootFrequencyBox.get());
	rootFrequencyBox->setEditable(false, true);
	rootFrequencyBox->onEditorShow = [&]() { rootFrequencyBackup = rootFrequencyBox->getText(); };
	rootFrequencyBox->onTextChange = [&]() { tuningReferenceEdited(); };

	auto rootFrequencyLabel = labels.add(new juce::Label("rootFrequencyLabel", "Root Frequency:"));
	rootFrequencyLabel->setJustificationType(juce::Justification::centredRight);
	rootFrequencyLabel->attachToComponent(rootFrequencyBox.get(), true);
	addAndMakeVisible(rootFrequencyLabel);

	auto mappingButtonCallback = [&]() { mappingTypeButtonClicked(); };

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

    setMappedTuning(tuningIn);
}

MappingPanel::~MappingPanel()
{
	periodicMappingButton = nullptr;
	linearMappingButton = nullptr;
	rootFrequencyBox = nullptr;
	rootMidiNoteBox = nullptr;
	rootMidiChannelBox = nullptr;
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

	juce::FlexBox mappingBox;
	mappingBox.flexDirection = juce::FlexBox::Direction::column;

	int referenceControlWidth = w * 0.18;
	int referenceWidth = w * 0.38;
	int referenceLabelWidth = w * 0.15;
	juce::FlexItem::Margin referenceMargin(0, 0, 0, referenceLabelWidth);

	mappingBox.items.add(juce::FlexItem(referenceControlWidth, controlHeight, *rootMidiChannelBox).withMargin(referenceMargin));
	mappingBox.items.add(juce::FlexItem(referenceControlWidth, controlHeight, *rootMidiNoteBox).withMargin(referenceMargin));
	mappingBox.items.add(juce::FlexItem(referenceControlWidth, controlHeight, *rootFrequencyBox).withMargin(referenceMargin));

	int buttonWidth = juce::roundToInt(w * 0.15);
	int buttonHeight = juce::roundToInt(controlHeight * 0.6);

	juce::FlexBox mappingRow;
	mappingRow.justifyContent = juce::FlexBox::JustifyContent::flexStart;
	mappingRow.items.add(juce::FlexItem(buttonWidth, buttonHeight, *linearMappingButton));
	mappingRow.items.add(juce::FlexItem(buttonWidth, buttonHeight, *periodicMappingButton));

	mappingBox.items.add(juce::FlexItem(buttonWidth * 2, buttonHeight, mappingRow).withMargin(referenceMargin));

	mappingBox.performLayout(getLocalBounds());
}

void MappingPanel::setMappedTuning(MappedTuningTable* tuningIn)
{

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
	auto tuning = mappedTuning->getTuning();
	auto mapping = mappedTuning->getMapping();
	
	rootMidiChannelBox->setText(juce::String(mapping->getRootMidiChannel()), juce::NotificationType::dontSendNotification);
	rootMidiNoteBox->setText(juce::String(mapping->getRootMidiNote()), juce::NotificationType::dontSendNotification);
	rootFrequencyBox->setText(juce::String(tuning->getRootFrequency()), juce::NotificationType::dontSendNotification);}

void MappingPanel::tuningReferenceEdited()
{
	MappedTuningTable::Root newRoot;

	auto channelInput = rootMidiChannelBox->getText().trim();
	int channelValue = -1;
	if (channelInput.containsOnly("0123456789"))
	{
		channelValue = channelInput.getIntValue();
		if (channelValue >= 1 && channelValue <= 16)
			newRoot.midiChannel = channelValue;
	}
	if (channelValue < 0)
	{
		rootMidiChannelBox->setText(rootChannelBackup, juce::NotificationType::dontSendNotification);
	}

	auto noteInput = rootMidiNoteBox->getText().trim();
	int noteValue = -1;
	if (noteInput.containsOnly("0123456789"))
	{
		noteValue = noteInput.getIntValue();
		if (noteValue >= 0 && noteValue < 128)
			newRoot.midiNote = noteValue;
	}
	if (noteValue < 0)
	{
		rootMidiNoteBox->setText(rootNoteBackup, juce::NotificationType::dontSendNotification);
	}

	auto freqInput = rootFrequencyBox->getText().trim();
	if (freqInput.endsWith("hz"))
		freqInput = freqInput.substring(0, freqInput.length() - 3).trim();

	double freqValue = -1;
	if (freqInput.containsOnly("0123456789."))
	{
		freqValue = freqInput.getDoubleValue();
		if (freqValue >= 8.0 && freqValue < 14000.0)
			newRoot.frequency = freqValue;

		rootFrequencyBox->setText(freqInput + " hz", juce::NotificationType::dontSendNotification);
	}
	if (freqValue < 0)
	{
		rootFrequencyBox->setText(rootFrequencyBackup, juce::NotificationType::dontSendNotification);
	}

	tuningWatchers.call(&TuningWatcher::targetMappedTuningRootChanged, this, newRoot);
}

